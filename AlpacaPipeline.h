#pragma once 
#include "IDataPipeline.h"
#include "IAsyncPublisher.h"
#include "StreamData.h"
#include "JSONFileParser.h"

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
    AlpacaPipeline(JSONFileParser& file, std::string  accountJSONKey, std::string _urlPath, int _port){  // pathUrl = /v2/{source} iex or sip to {source} iex is all you get without paying for subscription 
        urlPath = _urlPath;
        port = _port;
        url = file.getSubObjectValue(accountJSONKey,"WSURL");
        apiKey = file.getSubObjectValue(accountJSONKey,"APIKey");
        apiSecretKey = file.getSubObjectValue(accountJSONKey,"APISecretKey");
        connect();
    };
    virtual void connect(){ // do only once to start up websocket
        session = new Poco::Net::HTTPSClientSession(url, port);
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request("GET", urlPath);
        request.add("APCA-API-KEY-ID",apiKey);
        request.add("APCA-API-SECRET-KEY",apiSecretKey);
        websocket = new Poco::Net::WebSocket(*session,request,response);
    //std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl; // check websocket creation status
    };
    void notifyAll(std::shared_ptr<IStreamData> data){ // work in progress send them the buffer? also must filter so subs only get data for symbols they want
        for (auto &&i : subscribers){
            i.first->notify(data);
        }
    };
    virtual void sendRequest(){ // resend request when subs sub to new symbol
        Poco::Net::HTTPRequest request("GET", urlPath);
        request.add("APCA-API-KEY-ID",apiKey);
        request.add("APCA-API-SECRET-KEY",apiSecretKey);
        //request.add("Accept-Encoding","gzip,deflate"); // i guess I don't need to deflate it
        std::string sym;
        for (auto &&i : symbols){
            sym = sym + "\"" + i + "\"";
        }
        std::string payload("{\"action\":\"subscribe\",\"trades\":[" + sym + "],\"quotes\":[" + sym + "],\"bars\":[" + sym + "]}");


        // filter : trade,quote,summary,timesale,tradex default is all
        int flags = Poco::Net::WebSocket::FRAME_TEXT;
        websocket->sendFrame(payload.c_str(),payload.length(),flags);
    //std::cout << payload << std::endl;
    };
    virtual void subscribeToDataStream(std::string symbol, ISubscriber* subscriber){ // allow subscribers to specify the symbol of data to receive
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
            buffer[length] = '\0'; // add nullterminator to end of string to prevent grabage at end
            //std::istringstream s1(buffer);
            //std::istream is(s1.rdbuf());
            //std::stringstream ss;
            // ss << buffer; // i guess i don't need to deflate it???
            // Poco::InflatingInputStream inflater(ss, Poco::InflatingStreamBuf::STREAM_GZIP);
            // inflater.read(buffer,10000);
            
            //std::stringstream ss(buffer);
            std::string output(buffer);
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(output);
            Poco::JSON::Array::Ptr arr = result.extract<Poco::JSON::Array::Ptr>();
            //parse into different data types
        //std::cout << output <<std::endl;
            for(std::size_t i = 0; i < arr->size(); i++){ // go through each json object in array
                Poco::JSON::Object::Ptr object = arr->getObject(i); 
                std::string type = object->get("T").toString();
                if(type == "t"){
                    std::string symbol, exchangeCode, price, tradeSize, time;
                    symbol = object->get("S").toString();
                    exchangeCode = object->get("x").toString();
                    price = object->get("p").toString();
                    tradeSize = object->get("s").toString();
                    time = object->get("t").toString();
                    // tradeId, tradeCondition, and tape are not captured as tradier doesn't have those
                    notifyAll(std::shared_ptr<TradeData>(new TradeData(symbol,exchangeCode,price,tradeSize,time)));
                }
                else if(type == "q"){
                    std::string symbol, bidPrice, bidSize, bidExchange, bidTime, askPrice, askSize, askExchange, askTime; 
                    symbol = object->get("S").toString();
                    bidPrice = object->get("bp").toString();
                    bidSize = object->get("bs").toString();
                    bidExchange = object->get("bx").toString();
                    bidTime = object->get("t").toString();
                    askPrice = object->get("ap").toString();
                    askSize = object->get("as").toString();
                    askExchange = object->get("ax").toString();
                    askTime = object->get("t").toString(); // put for both askTime and bidTime as tradier has two time fields but alpaca only one
                    // tradesize, quote Condition, and tape are not captured as tradier doesn't have those
                    notifyAll(std::shared_ptr<QuoteData>(new QuoteData(symbol,bidPrice,bidSize,bidExchange,bidTime,askPrice,askSize,askExchange,askTime)));
                }
                else{
                    //give the data in an other data object 
                    notifyAll(std::shared_ptr<OtherData>(new OtherData(output))); // trash until I find use for these other data feeds i get for free from pipeline
                    // output may have more data then it should but it will give the object all the data to rep the unprocessed JSON object
                }
            }
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