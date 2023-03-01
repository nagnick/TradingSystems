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

class TradierPipeline: public IDataPipeline, public IAsyncPublisher{
    std::string url, sessionId, authScheme, apiKey;
    int port;
    char* buffer = new char[10000];
    Poco::Net::HTTPSClientSession* session = nullptr;
    Poco::Net::WebSocket* websocket = nullptr;
    std::vector<ISubscriber*> subscribers;
    void notifyAll(){ // work in progress send them the buffer
        for (auto &&i : subscribers){
            i->notify();
        }
    }
    public:
    TradierPipeline(std::string _url, std::string _sessionId, std::string _authScheme, std::string _apiKey, int _port):url(_url),sessionId(_sessionId),authScheme(_authScheme)
    ,apiKey(_apiKey),port(_port){
        connect();
    }
    virtual void connect(){
        session = new Poco::Net::HTTPSClientSession(url, port);
        Poco::Net::HTTPRequest request("GET", "/v1/accounts/events"); // url path or actuall path for websocket
        Poco::Net::HTTPResponse response;
        request.setKeepAlive(true);
        request.setContentType("application/json");
        request.add("Accept","application/json" ); // required by tradier does not read content Type field.
        request.setCredentials(authScheme, apiKey);
        request.add("sessionid", sessionId);
        request.add("events","[\"orders\"]"); 
        //request.add("symbols","[\"APPL\"]");// list of symbols you wish to receive market data from
        websocket = new Poco::Net::WebSocket(*session,request,response);
        
    }
    virtual void loop(){
        int flags;
        while(running){// a spinning polling loop
            websocket->receiveFrame((void*)buffer,10000,flags);
            std::cout << buffer << std::endl;
            notifyAll();
        }
    };
    virtual void start(){
        running = true;
        thread = std::thread(&TradierPipeline::loop, this);
    }
    virtual void subscribeToDataStreams(std::string stream){}; // maybe allow subscribers to specify their items of interest 
    virtual void subscribe(ISubscriber* subscriber) {
        subscribers.push_back(subscriber);
    };
    virtual void unSubscribe(ISubscriber* subscriber){
        for (auto i = subscribers.begin(); i != subscribers.end(); i++)
        {
            if((*i) == subscriber){
                subscribers.erase(i);
            }
        }
        
    };
    virtual ~TradierPipeline(){
        stop();
    };
};