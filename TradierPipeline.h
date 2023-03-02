#pragma once
#include "IDataPipeline.h"
#include "IAsyncPublisher.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/WebSocket.h"
#include <vector>
#include <map>

class TradierPipeline: public IDataPipeline, public IAsyncPublisher{
    std::string url, sessionId, authScheme, apiKey, urlPath;
    int port;
    char* buffer = new char[10000];
    Poco::Net::HTTPSClientSession* session = nullptr;
    Poco::Net::WebSocket* websocket = nullptr;
    std::map<ISubscriber*, std::vector<std::string>> subscribers;
    std::vector<std::string> symbols;
    void notifyAll(std::string payload){ // work in progress send them the buffer? also must filter so subs only get data for symbols they want
        for (auto &&i : subscribers){
            i.first->notify(payload);
        }
    }
    public:
    TradierPipeline(std::string _url, std::string _sessionId, std::string _authScheme, std::string _apiKey,std::string _urlPath, int _port):url(_url),sessionId(_sessionId)
    ,authScheme(_authScheme),apiKey(_apiKey), urlPath(_urlPath),port(_port){
        connect();
    }
    virtual void connect(){ // do only once to start up websocket
        session = new Poco::Net::HTTPSClientSession(url, port);
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request("GET", urlPath);
        request.add("Accept","application/json" ); // required by tradier does not read content Type field.
        request.setCredentials(authScheme, apiKey);
        request.add("sessionid", sessionId);
        websocket = new Poco::Net::WebSocket(*session,request,response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl; // check websocket creation status
    }
    virtual void sendRequest(){ // resend request when subs sub to new symbol
         Poco::Net::HTTPRequest request("GET", urlPath);
        request.add("Accept","application/json" ); // required by tradier does not read content Type field.
        request.setCredentials(authScheme, apiKey);
        request.add("sessionid", sessionId);
        std::string payload("{\"symbols\": [");
        for (auto &&i : symbols){
            payload = payload + "\"" + i + "\"";
        }
        payload = payload + "], \"sessionid\": \"" +sessionId +"\",\"linebreak\": true}";
        // filter : trade,quote,summary,timesale,tradex default is all
        int flags = Poco::Net::WebSocket::FRAME_TEXT;
        websocket->sendFrame(payload.c_str(),payload.length(),flags);
        std::cout << payload << std::endl;
    };
    virtual void loop(){
        int flags =  0;
        while(running){// a spinning polling loop
            websocket->receiveFrame((void*)buffer,10000,flags);
            //std::cout <<buffer << std::endl;
            notifyAll(std::string(buffer));
        }
    };
    virtual void start(){
        running = true;
        thread = std::thread(&TradierPipeline::loop, this);
    }
    // virtual void subscribeToDataStream(std::string stream, ISubscriber* subscriber){// allow subscribers to specify the stream of data of their symbols to receive 
    //     //filter : trade,quote,summary,timesale,tradex default is all
    //     // not implemented yet. maybe overkill on the granularity of the subscribe maybe best to leave it to subscriber to filter out themselves..
    // };
    virtual void subscribeToSymbolData(std::string symbol, ISubscriber* subscriber){// allow subscribers to specify the symbol of data to receive
        auto sub = subscribers.find(subscriber);
        if(sub != subscribers.end()){
            for (auto &&i : sub->second){
                if(i == symbol){
                    return; // if in sub list then must be in symbols vector
                }
            }
            sub->second.push_back(symbol);
            for (auto &&i : symbols){ // wasn't in sub list check main symbols vector
                if(i == symbol){
                    return;
                }
            }
            symbols.push_back(symbol);
            sendRequest(); // resend request with updated symbol list
        } // else fails quitely...
    }; 
    virtual void subscribe(ISubscriber* subscriber){ // done
        if(subscribers.find(subscriber) == subscribers.end()){
            subscribers.emplace(subscriber,std::vector<std::string>());
        }
    };
    virtual void unSubscribe(ISubscriber* subscriber){ // done
        for (auto i = subscribers.begin(); i != subscribers.end(); i++){
            if(i->first == subscriber){
                subscribers.erase(i);
            }
        }  
    };
    virtual ~TradierPipeline(){
        stop();
    };
};