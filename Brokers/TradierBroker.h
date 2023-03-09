#pragma once
#include "helpers/JSONFileParser.h"
#include "IBroker.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/URI.h"

#include <string>
using std::string;
// can trade stocks and options. no crypto 
class TradierBroker: public IBroker{ //  fix not safe as sending a new response clears out the input buffer find a fix..... as I want concurrent use of this I think
                                    // maybe use a command pattern(or just event obj?) to queue request and send responses as they come throuh pubsub interface???
    Poco::Net::HTTPSClientSession* session;
    string url,authScheme, apiKey, accountId;
    int port;
    public:
    TradierBroker(JSONFileParser& file, string accountJSONKey){
        url = file.getSubObjectValue(accountJSONKey,"URL");
        authScheme = file.getSubObjectValue(accountJSONKey,"AuthScheme");
        apiKey = file.getSubObjectValue(accountJSONKey,"APIKey");
        accountId = file.getSubObjectValue(accountJSONKey,"AccountNumber");
        port = 443;
        session = new Poco::Net::HTTPSClientSession(url , port);
        //thread = std::thread(&TradierBroker::start, this); // if i figure out how to do async https responses then inherite from IAsyncPublisher
        // thread = std::thread(&TradierBroker::start, std::ref(*this));
    };
    std::string sendRequestAndReturnString(string method, string urlPath){
        Poco::Net::HTTPRequest request(method, urlPath);
        //request.setHost(urlExtension, port);
        request.setKeepAlive(true);
        request.setContentLength(0);
        //request.setContentType("application/json");
        request.add("Accept","application/json"); // required by tradier does not read content Type field.
        request.setCredentials(authScheme, apiKey);
        session->sendRequest(request);
        //get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        int length = response.getContentLength();
        char* text = new char[length];
        s.getline(text,length);
        std::string res(text);
        delete[] text;
        return res;
    };
    Poco::JSON::Object::Ptr sendRequestAndReturnJSONResponse(string method, string urlPath){
        Poco::Net::HTTPRequest request(method, urlPath);
        //request.setHost(urlExtension, port);
        request.setKeepAlive(true);
        request.setContentLength(0);
        //request.setContentType("application/json"); // to work must not have this and content length of 0
        request.add("Accept","application/json" ); // required by tradier does not read content Type field.
        request.setCredentials(authScheme, apiKey);
        session->sendRequest(request);
        //get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        int length = response.getContentLength();
        std::stringstream buffer;
        buffer << s.rdbuf();
        Poco::JSON::Parser parser;
    //std::cout<< buffer.str() << std::endl;
        Poco::Dynamic::Var result = parser.parse(buffer.str());
        return  result.extract<Poco::JSON::Object::Ptr>(); // maybe make another version to return json arry ptr???
    };
    // user methods DONE
    void getUserInfo(){
        sendRequestAndReturnString("GET","/v1/user/profile");
    }
    // balance methods DONE
    void getBalances(){
        sendRequestAndReturnString("GET","/v1/accounts/" + accountId + "/balances");
    };
    // position methods DONE
    virtual void getAllPositions(){
        sendRequestAndReturnString("GET", "/v1/accounts/"+ accountId + "/positions");
    };
    virtual std::string getWebsocketSessionId(){ // need to retreive session Id to then pass to websocket(AKA TradierPipeline)
        Poco::JSON::Object::Ptr obj = sendRequestAndReturnJSONResponse("POST","/v1/markets/events/session");
        Poco::Dynamic::Var test = obj->get("stream");
        //std::cout << test.extract<Poco::JSON::Object::Ptr>()->get("sessionid").toString() << test.extract<Poco::JSON::Object::Ptr>()->get("url").toString() << std::endl;
        return test.extract<Poco::JSON::Object::Ptr>()->get("sessionid").toString();
    };
    // order methods WIP missing replace/modify order and some special kinds of orders
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop){
            // everything from price on is optional
        Poco::URI uri;
        uri.setPath("/v1/accounts/"+ accountId +"/orders");
        uri.addQueryParameter("class", "equity");
        uri.addQueryParameter("symbol",symbol);
        uri.addQueryParameter("side", side);
        uri.addQueryParameter("quantity",quantity);
        uri.addQueryParameter("type",type);
        uri.addQueryParameter("duration",duration); // minimum for order to work
        if(price != "NULL" && price != ""){
            uri.addQueryParameter("price",price);
        }
        if(stop != "NULL" && stop != ""){
            uri.addQueryParameter("stop",stop);
        }
        Poco::JSON::Object::Ptr result = sendRequestAndReturnJSONResponse("POST",uri.getPathAndQuery());//"/v1/accounts/"+ accountId +"/orders" path
        Poco::Dynamic::Var test = result->get("order");
        Poco::JSON::Object::Ptr subObject = test.extract<Poco::JSON::Object::Ptr>();
        return OrderResponse(subObject->get("id").toString(), subObject->get("status").toString());
    }
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop, string tag){
            // everything from price on is optional
        Poco::URI uri;
        uri.setPath("/v1/accounts/"+ accountId +"/orders");
        uri.addQueryParameter("class", "equity");
        uri.addQueryParameter("symbol",symbol);
        uri.addQueryParameter("side", side);
        uri.addQueryParameter("quantity",quantity);
        uri.addQueryParameter("type",type);
        uri.addQueryParameter("duration",duration); // minimum for order to work
        if(price != "NULL" && price != ""){
            uri.addQueryParameter("price",price);
        }
        if(stop != "NULL" && stop != ""){
            uri.addQueryParameter("stop",stop);
        }
        if(tag != "NULL" && tag != ""){
           uri.addQueryParameter("tag",tag);
        }
        Poco::JSON::Object::Ptr result = sendRequestAndReturnJSONResponse("POST",uri.getPathAndQuery());//"/v1/accounts/"+ accountId +"/orders" path
        Poco::Dynamic::Var test = result->get("order");
        Poco::JSON::Object::Ptr subObject = test.extract<Poco::JSON::Object::Ptr>();
        return OrderResponse(subObject->get("id").toString(), subObject->get("status").toString());
    }
    virtual void placeOptionOrder(string symbol, string option_symbol, string side, string quantity, string type,
        string duration, string price, string stop, string tag){
            // everything from price on is optional
        // sendRequestAndReturnString("POST","/v1/accounts/"+ accountId +"/orders",Poco::JSON::Object().set("class", "option").set("symbol",symbol).set(
        //     "option_symbol",option_symbol).set("side", side).set("quantity",quantity).set("type",type).set("duration",duration).set("price",price).set(
        //         "stop",stop).set("tag",tag));
    }
    virtual void cancelOrderByOrderId(string order_id){
        sendRequestAndReturnString("DELETE", "/v1/accounts/"+ accountId + "/orders/" + order_id);
    };
    virtual void getClock(){ // serves the current market timestamp, whether or not the market is currently open, as well as the times of the next market open and close.
        sendRequestAndReturnString("GET", "/v1/markets/clock");
    }; 
    // missing could be added: ALL marketData methods(leave for data pipeline websocket?), Watchlist, gain/loss, and History methods

    ~TradierBroker(){
    };
};
