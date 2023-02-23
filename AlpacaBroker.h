#pragma once
#include "JSONFileParser.h"
#include "IBroker.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include <string>

class AlpacaBroker: public IBroker{
    Poco::Net::HTTPSClientSession* session;
    std::string url, key, secretKey;
    int port;
    std::string authScheme, authInfo;
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
        request.setContentLength(0);
        request.setContentType("application/json");
        request.add("APCA-API-KEY-ID",key);
        request.add("APCA-API-SECRET-KEY",secretKey);
        session->sendRequest(request);

        // get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        char* text = new char[200];
        s.getline(text,200);
        std::cout << text;
        delete[] text;
    };
        
};