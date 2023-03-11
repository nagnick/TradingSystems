#pragma once
#include "helpers/JSONFileParser.h"
#include "IBroker.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"

#include <string>
using std::string;
class AlpacaBroker: public IBroker{ // can trade stocks and crypto. no options
    Poco::Net::HTTPSClientSession* session;
    std::string url, key, secretKey;
    int port;
    public:
    AlpacaBroker(JSONFileParser& file, std::string accountJSONKey){
        url = file.getSubObjectValue(accountJSONKey,"URL");
        key = file.getSubObjectValue(accountJSONKey,"APIKey");
        secretKey = file.getSubObjectValue(accountJSONKey,"APISecretKey");
        port = 443;
        session = new Poco::Net::HTTPSClientSession(url , port);
        session->setKeepAlive(true);
    };
    std::string sendRequestAndReturnString(std::string method, std::string urlPath, Poco::JSON::Object json){ // for debug/testing
        Poco::Net::HTTPRequest request(method, urlPath);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(ss.str().size());
        request.setContentType("application/json");
        request.add("APCA-API-KEY-ID",key);
        request.add("APCA-API-SECRET-KEY",secretKey);
        std::ostream& o = session->sendRequest(request);
        json.stringify(o);

        // get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        int length = response.getContentLength()+1; // plus one for null terminator?
        char* text = new char[length];
        s.getline(text,length);
        std::string res(text);
        delete[] text;
        return res;
    };
    Poco::JSON::Object::Ptr sendRequestAndReturnJSONObject(string method, string urlPath, Poco::JSON::Object json){ // final version
        Poco::Net::HTTPRequest request(method, urlPath);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(ss.str().size());
        request.setContentType("application/json");
        request.add("APCA-API-KEY-ID",key);
        request.add("APCA-API-SECRET-KEY",secretKey);
        std::ostream& o = session->sendRequest(request);
        json.stringify(o);
        //get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        int length = response.getContentLength();
        std::stringstream buffer;
        buffer << s.rdbuf();
    //std::cout<< buffer.str() << std::endl;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(buffer.str());
        return  result.extract<Poco::JSON::Object::Ptr>();
    };
    Poco::JSON::Array::Ptr sendRequestAndReturnJSONArray(string method, string urlPath, Poco::JSON::Object json){ // final version
        Poco::Net::HTTPRequest request(method, urlPath);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(ss.str().size());
        request.setContentType("application/json");
        request.add("APCA-API-KEY-ID",key);
        request.add("APCA-API-SECRET-KEY",secretKey);
        std::ostream& o = session->sendRequest(request);
        json.stringify(o);
        //get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        int length = response.getContentLength();
        std::stringstream buffer;
        buffer << s.rdbuf();
    //std::cout<< buffer.str() << std::endl;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(buffer.str());
        return  result.extract<Poco::JSON::Array::Ptr>();
    };
    int sendRequestAndReturnStatus(string method, string urlPath, Poco::JSON::Object json){ // final version
        Poco::Net::HTTPRequest request(method, urlPath);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(ss.str().size());
        request.setContentType("application/json");
        request.add("APCA-API-KEY-ID",key);
        request.add("APCA-API-SECRET-KEY",secretKey);
        std::ostream& o = session->sendRequest(request);
        json.stringify(o);
        //get response
        Poco::Net::HTTPResponse response;
        session->receiveResponse(response);
        //std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        return response.getStatus();
    };
    // account methods DONE
    virtual BalanceResponse getBalance(){ // get account returns balance info
        Poco::JSON::Object::Ptr ptr = sendRequestAndReturnJSONObject("GET" ,"/v2/account",Poco::JSON::Object());
        return BalanceResponse(ptr->get("cash").toString(),ptr->get("buying_power").toString());
        //std::cout << sendRequestAndReturnString("GET" ,"/v2/account",Poco::JSON::Object())<< std::endl;
    };
    // order methods WIP missing replace/modify order and placeOrder needs clean up or splitting up....
    void getOrder(){
        sendRequestAndReturnString("GET", "/v2/orders", Poco::JSON::Object());
        // default order query more options may be specified
    };
    virtual OrderResponse placeEquityOrder(string symbol, string side, string qty, string type,
        string duration, string price, string stop){ //DONE
            Poco::JSON::Object obj;
            obj.set("symbol", symbol).set("qty",qty).set("side",side).set("type",type).set("time_in_force",duration); // minimum for order to work
            if(price != "NULL" && price != ""){
                obj.set("limit_price",price);
            }
            if(stop != "NULL" && stop != ""){
                obj.set("stop_price",stop);
            }
            //set("trail_percent","NULL").set("trail_price","NULL");  might have to add these fields later for more complex orders
            Poco::JSON::Object::Ptr result = sendRequestAndReturnJSONObject("POST", "/v2/orders",obj);
        //std::cout << result->get("id").toString() << std::endl;
            return OrderResponse(result->get("id").toString(),result->get("status").toString());
    }
    void placeOrder(string symbol, string qty, string notional, string side, string type,
     string time_in_force, string limit_price, string stop_price, string trail_price,
     string trail_percent, string extended_hours, string client_order_id, string order_class,
     string take_profit, string stop_loss){ // https://alpaca.markets/docs/api-references/trading-api/orders/
     // from extended_hours on the fields are optional
        sendRequestAndReturnString("POST", "/v2/orders", Poco::JSON::Object().set("symbol", symbol).set("notional",notional).set("qty",qty).set("side",side).set(
            "type",type).set("time_in_force",time_in_force).set("limit_price",limit_price).set("stop_price",stop_price).set("trail_price",trail_price).set(
                "trail_percent",trail_percent).set("extended_hours",extended_hours).set("client_order_id",client_order_id).set("order_class",order_class).set(
                    "take_profit",take_profit).set("stop_loss",stop_loss)
        ); 
    };
    void getOrderByOrderId(string order_id){
        sendRequestAndReturnString("GET", "/v2/orders/"+order_id, Poco::JSON::Object());
    }
    void getOrderByClientOrderId(string client_order_id){
        sendRequestAndReturnString("GET", "/v2/orders:by_client_order_id", Poco::JSON::Object().set("client_order_id", client_order_id));
    };
    void cancelAllOrders(){
        sendRequestAndReturnString("DELETE", "/v2/orders",Poco::JSON::Object());
    };
    OrderResponse cancelOrderByOrderId(string order_id){ // DONE
        // this call only returns a status code
        int status = sendRequestAndReturnStatus("DELETE", "/v2/orders/" + order_id, Poco::JSON::Object());
        if(status == 204){
            return OrderResponse(order_id,"ok");
        }
        else{
            return OrderResponse(order_id,"rejected");
        }
    };
    // position methods DONE
    virtual std::vector<PositionResponse> getAllPositions(){ //DONE
        std::vector<PositionResponse> result;
        Poco::JSON::Array::Ptr array = sendRequestAndReturnJSONArray("GET", "/v2/positions", Poco::JSON::Object());
        for(std::size_t i = 0; i < array->size(); i++){ // go through each json object in array
                Poco::JSON::Object::Ptr object = array->getObject(i);
                result.push_back(PositionResponse(object->get("symbol").toString(),object->get("qty").toString(),
                object->get("cost_basis").toString(),object->get("asset_id").toString()));
        }
        return result;
    };
    void getPositionBySymbol(string symbol){
        sendRequestAndReturnString("GET", "/v2/positions/" + symbol, Poco::JSON::Object());
    };
    void closeAllPositions(bool cancel_orders){ //If true is specified, cancel all open orders before liquidating all positions.
        sendRequestAndReturnString("DELETE", "/v2/positions",Poco::JSON::Object().set("cancel_orders", cancel_orders));
    };
    void closePosition(string symbolORasset_id){
        sendRequestAndReturnString("DELETE", "/v2/positions/" + symbolORasset_id, Poco::JSON::Object());
    };
    void closePositionByQuantity(string symbolORasset_id, double qty){
        sendRequestAndReturnString("DELETE", "/v2/positions/" + symbolORasset_id, Poco::JSON::Object().set("qty", qty));
    };
    void closePositionByPercentage(string symbol_or_asset_id, double percentage){
        sendRequestAndReturnString("DELETE", "/v2/positions/" + symbol_or_asset_id, Poco::JSON::Object().set("percentage", percentage));
    };
    //asset methods (query for what you can trade on alpaca) DONE
    void getAsset(string status, string asset_class, string exchange){ // all are optional. optional = ""
        Poco::JSON::Object obj;
        if(status != ""){
            obj.set("status", status); //e.g. “active”. By default, all statuses are included.
        }
        if(asset_class != ""){
            obj.set("asset_class", asset_class); //Defaults to us_equity.
        }
        if( exchange != ""){
            obj.set("exchange", exchange); //AMEX, ARCA, BATS, NYSE, NASDAQ, NYSEARCA or OTC
        }
        sendRequestAndReturnString("GET", "/v2/assets", obj);
    };
    void getAsset(string symbol_or_asset_id){
        sendRequestAndReturnString("GET", "/v2/assets/" + symbol_or_asset_id, Poco::JSON::Object());
    };
    // clock methods DONE
    virtual void getClock(){ // serves the current market timestamp, whether or not the market is currently open, as well as the times of the next market open and close.
        sendRequestAndReturnString("GET", "/v2/clock", Poco::JSON::Object());
    };
    // missing could be added: Watchlist, Calendar, Corporate Actions Announcements, Account Configurations, Account Activities, and Portfolio History methods
};