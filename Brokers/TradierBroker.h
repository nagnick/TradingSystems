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
    std::string sendRequestAndReturnString(string method, string urlPath){ // for testing/debug
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
        int length = response.getContentLength() + 1;//should I add 1 for nullterminator?
        char* text = new char[length];
        s.getline(text,length);
        std::string res(text);
        delete[] text;
        return res;
    };
    Poco::JSON::Object::Ptr sendRequestAndReturnJSONObject(int& status, string method, string urlPath){ // final version
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
    //std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        status = response.getStatus();
        //int length = response.getContentLength();
        if(status == 200){ // initial error handling return empty json object if error status returned parse would throw exception with empty response
        // error handling should be improved
            std::stringstream buffer;
            buffer << s.rdbuf();
            Poco::JSON::Parser parser;
        //std::cout<< buffer.str() << std::endl;
            Poco::Dynamic::Var result = parser.parse(buffer.str());
            return  result.extract<Poco::JSON::Object::Ptr>();
        }
        return nullptr; // error status return nothing
    };
    // Poco::JSON::Array::Ptr sendRequestAndReturnJSONArray(string method, string urlPath){ // not required tradier puts arrays in object so only need object version
    //     Poco::Net::HTTPRequest request(method, urlPath);
    //     //request.setHost(urlExtension, port);
    //     request.setKeepAlive(true);
    //     request.setContentLength(0);
    //     //request.setContentType("application/json"); // to work must not have this and content length of 0
    //     request.add("Accept","application/json" ); // required by tradier does not read content Type field.
    //     request.setCredentials(authScheme, apiKey);
    //     session->sendRequest(request);
    //     //get response
    //     Poco::Net::HTTPResponse response;
    //     std::istream& s = session->receiveResponse(response);
    //     std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
    //     int length = response.getContentLength();
    //     std::stringstream buffer;
    //     buffer << s.rdbuf();
    //     Poco::JSON::Parser parser;
    // std::cout<< buffer.str() << std::endl;
    //     Poco::Dynamic::Var result = parser.parse(buffer.str());
    //     return  result.extract<Poco::JSON::Array::Ptr>();
    // };
    // user methods DONE
    void getUserInfo(){
        sendRequestAndReturnString("GET","/v1/user/profile");
    }
    // balance methods DONE
    virtual BalanceResponse getBalance(){
       // std::cout<< sendRequestAndReturnString("GET","/v1/accounts/" + accountId + "/balances") << std::endl;
        int status = 0;
        Poco::JSON::Object::Ptr ptr = sendRequestAndReturnJSONObject(status, "GET","/v1/accounts/" + accountId + "/balances");
        Poco::JSON::Object::Ptr obj = ptr->getObject("balances");
        return BalanceResponse(obj->get("total_cash").toString(),obj->get("total_cash").toString()); // fix when account type is different ex: margin vs cash account they will vary
    };
    // position methods DONE
    virtual std::vector<PositionResponse> getAllPositions(){ //DONE
        int status = 0;
        std::vector<PositionResponse> result;
        Poco::JSON::Object::Ptr obj = sendRequestAndReturnJSONObject(status,"GET", "/v1/accounts/"+ accountId + "/positions");
        Poco::JSON::Object::Ptr positions = obj->getObject("positions");
        if(positions){ // if nullptr then no positions return empty vector
            Poco::Dynamic::Var position = positions->get("position"); // if one is single object else is an array
            if(position.isArray()){
                Poco::JSON::Array::Ptr array = position.extract<Poco::JSON::Array::Ptr>();
                for(std::size_t i = 0; i < array->size(); i++){ // go through each json object in array
                        Poco::JSON::Object::Ptr object = array->getObject(i);
                        result.push_back(PositionResponse(object->get("symbol").toString(),object->get("quantity").toString(),
                        object->get("cost_basis").toString(),object->get("id").toString()));
                }
            }
            else{
                Poco::JSON::Object::Ptr object = position.extract<Poco::JSON::Object::Ptr>();
                result.push_back(PositionResponse(object->get("symbol").toString(),object->get("quantity").toString(),
                        object->get("cost_basis").toString(),object->get("id").toString()));
            }
        }
        return result;

    };
    virtual std::string getWebsocketSessionId(){ // need to retreive session Id to then pass to websocket(AKA TradierPipeline)
        int status = 0;
        Poco::JSON::Object::Ptr obj = sendRequestAndReturnJSONObject(status, "POST","/v1/markets/events/session");
        Poco::Dynamic::Var test = obj->get("stream");
        //std::cout << test.extract<Poco::JSON::Object::Ptr>()->get("sessionid").toString() << test.extract<Poco::JSON::Object::Ptr>()->get("url").toString() << std::endl;
        return test.extract<Poco::JSON::Object::Ptr>()->get("sessionid").toString();
    };
    // order methods WIP missing replace/modify order and some special kinds of orders
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop){ //DONE
            // everything from price on is optional
        int status = 0;
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
        Poco::JSON::Object::Ptr result = sendRequestAndReturnJSONObject(status, "POST", uri.getPathAndQuery());//"/v1/accounts/"+ accountId +"/orders" path
        Poco::Dynamic::Var test = result->get("order");
        Poco::JSON::Object::Ptr subObject = test.extract<Poco::JSON::Object::Ptr>();
        return OrderResponse(subObject->get("id").toString(), subObject->get("status").toString());
    }
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop, string tag){
            // everything from price on is optional
        int status = 0;
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
        Poco::JSON::Object::Ptr result = sendRequestAndReturnJSONObject(status,"POST",uri.getPathAndQuery());//"/v1/accounts/"+ accountId +"/orders" path
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
    virtual OrderResponse cancelOrderByOrderId(string order_id){ // Done
        int status = 0;
        Poco::URI uri;
        uri.setPath("/v1/accounts/"+ accountId + "/orders/" + order_id);
        Poco::JSON::Object::Ptr result = sendRequestAndReturnJSONObject(status,"DELETE", uri.getPathAndQuery());
        if(result){
            Poco::Dynamic::Var test = result->get("order");
            Poco::JSON::Object::Ptr subObject = test.extract<Poco::JSON::Object::Ptr>();
            return OrderResponse(subObject->get("id").toString(), subObject->get("status").toString());
        }
        else{ // errored return error
            //std::cout<< "status: " << status << std::endl; // = 400 already canceled = 401 not found
            if(status == 401){
                return OrderResponse(order_id,"Order Id not found");
            }
            return OrderResponse(order_id,"Unprocessable/rejected");
        }
    };
    virtual ClockResponse getClock(){ // serves the current market timestamp, whether or not the market is currently open, as well as the times of the next market open and close.
        int status = 0;
        Poco::JSON::Object::Ptr ptr = sendRequestAndReturnJSONObject(status,"GET", "/v1/markets/clock");//->stringify(std::cout);
        ptr = ptr->getObject("clock");
        std::string timestamp = ptr->get("date").toString() + "T" + ptr->get("timestamp").toString(); // need to do proper time stamp conversion to match Alpaca
        bool is_open = ptr->get("state").toString() == "open";
        return ClockResponse(timestamp,is_open);
    }; 
    // missing could be added: ALL marketData methods(leave for data pipeline websocket?), Watchlist, gain/loss, and History methods
    virtual std::vector<BarResponse> getDailyHistoricalBars(std:: string symbol, std::string start, std::string end){ // start and end in YYYY-MM-DD representation
        std::vector<BarResponse> bars;
        int status = 0;
        Poco::URI uri;
        uri.setPath("/v1/markets/history");
        uri.addQueryParameter("symbol", symbol);
        uri.addQueryParameter("start", start);
        uri.addQueryParameter("end", end);
        Poco::JSON::Object::Ptr ptr = sendRequestAndReturnJSONObject(status,"GET", uri.getPathAndQuery());
        if(ptr){
            ptr = ptr->getObject("history");
            Poco::JSON::Array::Ptr array = ptr->getArray("day");
            for(std::size_t i = 0; i < array->size(); i++){
                ptr = array->getObject(i);
                bars.push_back(BarResponse(symbol,ptr->get("open"),ptr->get("close"),ptr->get("low"),ptr->get("high"),ptr->get("volume"),ptr->get("date")));
            }
        }
        return bars;
    };

    ~TradierBroker(){
    };
};
