#include "JSONFileParser.h"
#include "IBroker.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include "IAsyncPublisher.h"
#include <string>
using std::string;

class TradierBroker: public IBroker{
    Poco::Net::HTTPSClientSession* session;
    string url,authScheme, apiKey, accountId;
    int port;
    public:
    TradierBroker(JSONFileParser& file){
        url = file.getSubObjectValue("tradier","URL");
        authScheme = file.getSubObjectValue("tradier","AuthScheme");
        apiKey = file.getSubObjectValue("tradier","APIKey");
        accountId = file.getSubObjectValue("tradier","AccountNumber");
        port = 443;
        session = new Poco::Net::HTTPSClientSession(url , port);
        //thread = std::thread(&TradierBroker::start, this); // if i figure out how to do async https responses then inherite from IAsyncPublisher
        // thread = std::thread(&TradierBroker::start, std::ref(*this));
    };
    void sendRequest(string method, string urlPath, Poco::JSON::Object json){
       Poco::Net::HTTPRequest request(method, urlPath);
        //request.setHost(urlExtension, port);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(ss.str().size());
        request.setContentType("application/json");
        request.add("Accept","application/json" ); // required by tradier does not read content Type field.
        request.setCredentials(authScheme, apiKey);
        std::ostream& o = session->sendRequest(request);
        json.stringify(o);  // write json to ostream of request
        //get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        int length = response.getContentLength();
        char* text = new char[length];
        s.getline(text,length);
        std::cout << text << std::endl;
        delete[] text;
    };
    // user methods DONE
    void getUserInfo(){
        sendRequest("GET","/v1/user/profile",Poco::JSON::Object());
    }
    // balance methods DONE
    void getBalances(){
        sendRequest("GET","/v1/accounts/" + accountId + "/balances",Poco::JSON::Object());
    };
    // position methods DONE
    virtual void getAllPositions(){
        sendRequest("GET", "/v1/accounts/"+ accountId + "/positions",Poco::JSON::Object());
    };
    // create websocket methods WIP
    virtual void getStreamSessionId(){ // need to retreive session Id to then pass to websocket(AKA TradierPipeline)
        sendRequest("POST","/v1/markets/events/session", Poco::JSON::Object());
    }
    // order methods WIP missing replace/modify order and some special kinds of orders
    virtual void placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop, string tag){
            // everything from price on is optional
        sendRequest("POST","/v1/accounts/"+ accountId +"/orders",Poco::JSON::Object().set("class", "equity"));
    }
    virtual void placeOptionOrder(string symbol, string option_symbol, string side, string quantity, string type,
        string duration, string price, string stop, string tag){
            // everything from price on is optional
        sendRequest("POST","/v1/accounts/"+ accountId +"/orders",Poco::JSON::Object().set("class", "option"));
    }
    virtual void cancelOrderByOrderId(string order_id){
        sendRequest("DELETE", "/v1/accounts/"+ accountId + "/orders/" + order_id, Poco::JSON::Object());
    };
    // missing could be added: ALL marketData methods(leave for data pipeline websocket?), Watchlist, gain/loss, and History methods




    // virtual void start(){ // fix to notify subscribers instead of print it to screen
    //     while(running){ //  fix not safe as sending a new response clears out the input buffer find a fix..... as I want concurrent use of this I think
    //         //get response
    //         Poco::Net::HTTPResponse response;
    //         std::istream& s = session->receiveResponse(response);
    //         std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
    //         char* text = new char[200];
    //         s.getline(text,200);
    //         std::cout << text << std::endl;
    //         delete[] text;
    //         // char* text = new char[200];
    //         // session->receive(text,200);
    //         // std::cout << text;
    //         // delete[] text;
    //     }
    // };
    // virtual void subscribe(ISubscriber* subscriber){ // fix
    //     std:: cout << "subscribe not implemented" << std::endl;
    // }
    // virtual void unSubscribe(ISubscriber* subscriber){ // fix not a publisher at the momnet 
    //     std:: cout << "unSubscribe not implemented" << std::endl;
    // };
    ~TradierBroker(){
    };
};
