#pragma once
#include "IDataStream.h"
#include "helpers/IAsync.h"
#include "helpers/JSONFileParser.h"
#include "StreamData.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/WebSocket.h"
#include <vector>
#include <map>

class TradierStream: public IDataStream, public IAsync{
    std::string url, sessionId, authScheme, apiKey, urlPath;
    int port;
    char* buffer = new char[10000];
    Poco::Net::HTTPSClientSession* session = nullptr;
    Poco::Net::WebSocket* websocket = nullptr;
    std::map<IDataStreamSubscriber*, std::vector<std::string>> subscribers;
    std::vector<std::string> symbols;
    bool subscribedTo(IDataStreamSubscriber * sub ,std::string symbol){// checks if sub is subscribed to symbol
        std::vector<std::string> subedSymbols = subscribers.at(sub);
        for (auto &&i : subedSymbols){
            if(i == symbol){
                return true;
            }
        }
        return false;
    }
    void notifyAll(std::shared_ptr<IStreamData> data){ //filters so subs only get data for symbols they want
        std::string symbol = "";
        switch(data->getDataType()){
            case 0:{
                std::shared_ptr<TradeData> trade = std::dynamic_pointer_cast<TradeData,IStreamData>(data);
                symbol = trade->symbol;
                break;
            }
            case 1:{
                std::shared_ptr<QuoteData> quote = std::dynamic_pointer_cast<QuoteData,IStreamData>(data);
                symbol = quote->symbol;
                break;
            }
            default:{
                break;
            }
        }
        for (auto &&i : subscribers){
            if(symbol == "" || subscribedTo(i.first,symbol)){ // default data is sent
                i.first->notify(data);
            }
        }
    };

    public:
    TradierStream(JSONFileParser& file, std::string  accountJSONKey, std::string _sessionId, std::string _urlPath, int _port){ // urlPath = "/v1/markets/events"
        sessionId = _sessionId;
        urlPath = _urlPath;
        port = _port;
        url = file.getSubObjectValue(accountJSONKey,"WSURL");
        authScheme = file.getSubObjectValue(accountJSONKey,"AuthScheme");
        apiKey = file.getSubObjectValue(accountJSONKey,"APIKey");
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
    //std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl; // check websocket creation status
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
    //std::cout << payload << std::endl;
    };
    virtual void loop(){
        int flags =  0;
        while(running){// a spinning polling loop
            int length = websocket->receiveFrame((void*)buffer,10000,flags);
            buffer[length] = '\0'; // add nullterminator to end of string to prevent grabage at end
        //std::cout << "whole chunk:"<<buffer << "\nParts:" <<  std::endl;

            std::stringstream ss(buffer);
            std::string output;
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result;
            Poco::JSON::Object::Ptr object;
            //parse into different data types
            while(std::getline(ss,output,'\n')){ // each json object split by newline in tradier by default
        //std::cout << output <<std::endl;

                result =parser.parse(output);
                object = result.extract<Poco::JSON::Object::Ptr>();
                std::string type = object->get("type").toString();
                if(type == "trade"){
                    std::string symbol, exchangeCode, price, tradeSize, time;
                    symbol = object->get("symbol").toString();
                    exchangeCode = object->get("exch").toString();
                    price = object->get("price").toString();
                    tradeSize = object->get("size").toString();
                    time = object->get("date").toString();
                    // last price "last" and cumulative volume "cvol" are not captured as alpaca doesn't have those
                    notifyAll(std::shared_ptr<TradeData>(new TradeData(symbol,exchangeCode,price,tradeSize,time)));
                }
                else if(type == "quote"){
                    std::string symbol, bidPrice, bidSize, bidExchange, bidTime, askPrice, askSize, askExchange, askTime; 
                    symbol = object->get("symbol").toString();
                    bidPrice = object->get("bid").toString();
                    bidSize = object->get("bidsz").toString();
                    bidExchange = object->get("bidexch").toString();
                    bidTime = object->get("biddate").toString();
                    askPrice = object->get("ask").toString();
                    askSize = object->get("asksz").toString();
                    askExchange = object->get("askexch").toString();
                    askTime = object->get("askdate").toString();
                    // all parts of quote are captured
                    notifyAll(std::shared_ptr<QuoteData>(new QuoteData(symbol,bidPrice,bidSize,bidExchange,bidTime,askPrice,askSize,askExchange,askTime)));
                }
                else{
                    //give the data in an other data object 
                    notifyAll(std::shared_ptr<OtherData>(new OtherData(output)));  // trash until I find use for these other data feeds i get for free from pipeline
                    // output may have more data then it should but it will give the object all the data to rep the unprocessed JSON object
                }
            }
        }
    };
    // virtual void start(){
    //     running = true;
    //     thread = std::thread(&TradierPipeline::loop, this);
    // }
    // virtual void subscribeToDataStream(std::string stream, IDataStreamSubscriber* subscriber){// allow subscribers to specify the stream of data of their symbols to receive 
    //     //filter : trade,quote,summary,timesale,tradex default is all
    //     // not implemented yet. maybe overkill on the granularity of the subscribe maybe best to leave it to subscriber to filter out themselves..
    // };
    virtual void subscribeToDataStream(std::string symbol, IDataStreamSubscriber* subscriber){// allow subscribers to specify the symbol of data to receive
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
    virtual void subscribe(IDataStreamSubscriber* subscriber){ // done
        if(subscribers.find(subscriber) == subscribers.end()){
            subscribers.emplace(subscriber,std::vector<std::string>());
        }
    };
    virtual void unSubscribe(IDataStreamSubscriber* subscriber){ // done
        for (auto i = subscribers.begin(); i != subscribers.end(); i++){
            if(i->first == subscriber){
                subscribers.erase(i);
            }
        }  
    };
    virtual ~TradierStream(){
        stop();
    };
};