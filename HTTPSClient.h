// prototype class not used anymore to to major differences between brokers...
#pragma once
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/JSON/Object.h"
#include "Poco/Base64Encoder.h"
#include <string>
class HTTPSClient{ // pocoClientseccion wrapper
    Poco::Net::HTTPSClientSession* session;
    std::string url;
    int port;
    std::string authScheme, authInfo;
    public:
    HTTPSClient(std::string _url, int _port, std::string authScheme){
        url = _url;
        port = _port;
        session = new Poco::Net::HTTPSClientSession(url , port);
        this->authScheme = authScheme;
    };
    void setKeepAlive(bool alive){
        session->setKeepAlive(alive);
    };
    bool connected(){
        return session->connected();
    }
    void sendRequest(Poco::JSON::Object json, std::string method, std::string urlExtension){
        // send message
        Poco::Net::HTTPRequest request(method, urlExtension);
        //request.setHost(urlExtension, port);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(0); //ss.str().size()
        request.setContentType("application/json");
       request.setCredentials(authScheme, authInfo);
        //request.add("Authorization", "Bearer"); // add token to request??
        //request.setContentType("application/json");
        //std::ostream& o = session->sendRequest(request);
        //json.stringify(o);  // write json to ostream of request
        session->sendRequest(request);


        // get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        char* text = new char[200];
        s.getline(text,200);
        std::cout << text;
    };
};
// Poco::JSON::Object obj;
//     obj.set("name", name);
//     obj.set("url", repo);

/*HTTPClientSession session(uri.getHost(), uri.getPort());
HTTPRequest request(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
HTTPResponse response;

std::stringstream ss;
obj.stringify(ss);
request.setKeepAlive(true);
request.setContentLength(ss.str().size());
request.setContentType("application/json");  // definately set Content-Type right?

std::ostream& o = session.sendRequest(request);
obj.stringify(o);             // can confirm it is spitting out the valid 

std::cout << response.getStatus() << " " << response.getReason() << std::endl;*/