#pragma once 
#include "IDataPipeline.h"
#include "IAsyncPublisher.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/InflatingStream.h"
#include "Poco/StreamCopier.h"
#include <vector>
#include <map>

class AlpacaPipeline : public IDataPipeline, public IAsyncPublisher {
    std::string url, apiKey, apiSecretKey, urlPath;
    int port;
    char* buffer = new char[10000];

    Poco::Net::HTTPSClientSession* session = nullptr;
    Poco::Net::WebSocket* websocket = nullptr;
    std::map<ISubscriber*, std::vector<std::string>> subscribers;
    std::vector<std::string> symbols;
    public:
    AlpacaPipeline(std::string _url,std::string _apiKey, std::string _apiSecretKey,std::string _urlPath, int _port):url(_url)
    ,apiKey(_apiKey),apiSecretKey(_apiSecretKey), urlPath(_urlPath),port(_port){
        connect();
    };
    virtual void connect(){ // do only once to start up websocket
        session = new Poco::Net::HTTPSClientSession(url, port);
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request("GET", urlPath);
        request.add("APCA-API-KEY-ID",apiKey);
        request.add("APCA-API-SECRET-KEY",apiSecretKey);
        websocket = new Poco::Net::WebSocket(*session,request,response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl; // check websocket creation status
    };
    void notifyAll(std::string payload){ // work in progress send them the buffer? also must filter so subs only get data for symbols they want
        for (auto &&i : subscribers){
            i.first->notify(payload);
        }
    };
    virtual void sendRequest(){ // resend request when subs sub to new symbol
        Poco::Net::HTTPRequest request("GET", urlPath);
        request.add("APCA-API-KEY-ID",apiKey);
        request.add("APCA-API-SECRET-KEY",apiSecretKey);
        request.add("Accept-Encoding","gzip,deflate");
        std::string sym;
        for (auto &&i : symbols){
            sym = sym + "\"" + i + "\"";
        }
        std::string payload("{\"action\":\"subscribe\",\"trades\":[" + sym + "],\"quotes\":[" + sym + "],\"bars\":[" + sym + "]}");


        // filter : trade,quote,summary,timesale,tradex default is all
        int flags = Poco::Net::WebSocket::FRAME_TEXT;
        websocket->sendFrame(payload.c_str(),payload.length(),flags);
        std::cout << payload << std::endl;
    };
    virtual void subscribeToSymbolData(std::string symbol, ISubscriber* subscriber){
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
    virtual void subscribe(ISubscriber* subscriber){
        if(subscribers.find(subscriber) == subscribers.end()){
            subscribers.emplace(subscriber,std::vector<std::string>());
        }
    };
    virtual void unSubscribe(ISubscriber* subscriber){
        for (auto i = subscribers.begin(); i != subscribers.end(); i++){
            if(i->first == subscriber){
                subscribers.erase(i);
            }
        }  
    };
    virtual void loop(){
        int flags =  0;
        while(running){
            int length = websocket->receiveFrame((void*)buffer,10000,flags);
            std::istringstream s1(buffer);
            std::istream is(s1.rdbuf());
            std::stringstream ss;
            buffer[length] = '\0'; // add nullterminator to end of string to prevent grabage at end
            ss << buffer;
            Poco::InflatingInputStream inflater(ss, Poco::InflatingStreamBuf::STREAM_GZIP);
            inflater.read(buffer,10000);
            std::string temp(buffer);
            //std::cout << temp << std::endl;
            notifyAll(temp );
        }
    };
    virtual void start(){
        running = true;
        thread = std::thread(&AlpacaPipeline::loop, this);
    };
    ~AlpacaPipeline(){
        stop();
    }
};