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
    std::string sendRequestAndReturnString(std::string method, std::string urlPath, Poco::JSON::Object json){
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
        int length = response.getContentLength();
        char* text = new char[length];
        s.getline(text,length);
        std::string res(text);
        //std::cout << text;
        delete[] text;
        return res;
    };
    Poco::JSON::Object::Ptr sendRequestAndReturnJSONResponse(string method, string urlPath, Poco::JSON::Object json){
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
        std::cout << length << std::endl;
        std::stringstream buffer;
        buffer << s.rdbuf();
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(buffer.str());
        // use pointers to avoid copying
        return  result.extract<Poco::JSON::Object::Ptr>();
    };
    // account methods DONE
    void getAccount(){
        sendRequestAndReturnString("GET" ,"/v2/account",Poco::JSON::Object());
    };
    // order methods WIP missing replace/modify order and placeOrder needs clean up or splitting up....
    void getOrder(){
        sendRequestAndReturnString("GET", "/v2/orders", Poco::JSON::Object());
        // default order query more options may be specified
    };
    virtual void placeEquityOrder(string symbol, string side, string qty, string type,
        string duration, string price, string stop){
            sendRequestAndReturnString("POST", "/v2/orders", Poco::JSON::Object().set("symbol", symbol).set("qty",qty).set("side",side).set(
            "type",type).set("time_in_force",duration).set("limit_price",price).set("stop_price",stop)); 
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
    void cancelOrderByOrderId(string order_id){
        sendRequestAndReturnString("DELETE", "/v2/orders/" + order_id, Poco::JSON::Object());
    };
    // position methods DONE
    virtual void getAllPositions(){
        sendRequestAndReturnString("GET", "/v2/positions", Poco::JSON::Object());
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