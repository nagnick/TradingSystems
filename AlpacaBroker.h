#pragma once
#include "JSONFileParser.h"
#include "IBroker.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include <string>
using std::string;
class AlpacaBroker: public IBroker{
    Poco::Net::HTTPSClientSession* session;
    std::string url, key, secretKey;
    int port;
    public:
    AlpacaBroker(JSONFileParser& file){
        url = file.getSubObjectValue("alpaca","URL");
        key = file.getSubObjectValue("alpaca","APIKey");
        secretKey = file.getSubObjectValue("alpaca","APISecretKey");
        port = 443;
        session = new Poco::Net::HTTPSClientSession(url , port);
        session->setKeepAlive(true);
    };
    void sendRequest(std::string method, std::string urlPath, Poco::JSON::Object json){
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
        std::cout << text;
        delete[] text;
    };
    // account methods DONE
    void getAccount(){
        sendRequest("GET" ,"/v2/account",Poco::JSON::Object());
    };
    // order methods WIP missing replace/modify order and placeOrder needs clean up or splitting up....
    void getOrder(){
        sendRequest("GET", "/v2/orders", Poco::JSON::Object());
        // default order query more options may be specified
    };
    void placeOrder(string symbol, string qty, string notional, string side, string type,
     string time_in_force, string limit_price, string stop_price, string trail_price,
     string trail_percent, string extended_hours, string client_order_id, string order_class,
     string take_profit, string stop_loss){ // https://alpaca.markets/docs/api-references/trading-api/orders/
     // from extended_hours on the fields are optional
        sendRequest("POST", "/v2/orders", Poco::JSON::Object().set("symbol", symbol).set("notional",notional).set("qty",qty).set("side",side).set(
            "type",type).set("time_in_force",time_in_force).set("limit_price",limit_price).set("stop_price",stop_price).set("trail_price",trail_price).set(
                "trail_percent",trail_percent)
        ); 
    };
    void getOrderByOrderId(string order_id){
        sendRequest("GET", "/v2/orders/"+order_id, Poco::JSON::Object());
    }
    void getOrderByClientOrderId(string client_order_id){
        sendRequest("GET", "/v2/orders:by_client_order_id", Poco::JSON::Object().set("client_order_id", client_order_id));
    };
    void cancelAllOrders(){
        sendRequest("DELETE", "/v2/orders",Poco::JSON::Object());
    };
    void cancelOrderByOrderId(string order_id){
        sendRequest("DELETE", "/v2/orders/" + order_id, Poco::JSON::Object());
    };
    // position methods DONE
    virtual void getAllPositions(){
        sendRequest("GET", "/v2/positions", Poco::JSON::Object());
    };
    void getPositionBySymbol(string symbol){
        sendRequest("GET", "/v2/positions/" + symbol, Poco::JSON::Object());
    };
    void closeAllPositions(bool cancel_orders){ //If true is specified, cancel all open orders before liquidating all positions.
        sendRequest("DELETE", "/v2/positions",Poco::JSON::Object().set("cancel_orders", cancel_orders));
    };
    void closePosition(string symbolORasset_id){
        sendRequest("DELETE", "/v2/positions/" + symbolORasset_id, Poco::JSON::Object());
    };
    void closePositionByQuantity(string symbolORasset_id, double qty){
        sendRequest("DELETE", "/v2/positions/" + symbolORasset_id, Poco::JSON::Object().set("qty", qty));
    };
    void closePositionByPercentage(string symbol_or_asset_id, double percentage){
        sendRequest("DELETE", "/v2/positions/" + symbol_or_asset_id, Poco::JSON::Object().set("percentage", percentage));
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
        sendRequest("GET", "/v2/assets", obj);
    };
    void getAsset(string symbol_or_asset_id){
        sendRequest("GET", "/v2/assets/" + symbol_or_asset_id, Poco::JSON::Object());
    };
    // clock methods DONE
    void getClock(){ // serves the current market timestamp, whether or not the market is currently open, as well as the times of the next market open and close.
        sendRequest("GET", "/v2/clock", Poco::JSON::Object());
    }; 
    // missing could be added: Watchlist, Calendar, Corporate Actions Announcements, Account Configurations, Account Activities, and Portfolio History methods
};