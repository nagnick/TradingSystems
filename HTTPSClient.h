// this class will handle all network connectivity that involves HTTPS
#pragma once
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/JSON/Object.h"
#include <string>
class HTTPSClient{ // pocoClientseccion wrapper
    Poco::Net::HTTPSClientSession* session;
    std::string url;
    int port;
    public:
    HTTPSClient(std::string _url, int _port){
        url = _url;
        port = _port;
        session = new Poco::Net::HTTPSClientSession(url , port);
    };
    void setKeepAlive(bool alive){
        session->setKeepAlive(alive);
    };
    bool connected(){
        return session->connected();
    }
    void sendRequest(Poco::JSON::Object json){
        // send message
        std::string version = "1.1";
        Poco::Net::HTTPRequest request(version);
        request.setHost(url, port);
        request.setKeepAlive(true);
        std::stringstream ss;
        json.stringify(ss);

        request.setContentLength(ss.str().size());
        request.setContentType("application/json");
        //request.add("Authorization", "Bearer"); // add token to request??
        //request.setContentType("application/json");
        std::ostream& o = session->sendRequest(request);
        json.stringify(o);  // write json to ostream of request
        session->sendRequest(request);


        // get response
        Poco::Net::HTTPResponse response;
        std::istream& s = session->receiveResponse(response);
        std::cout << response.getStatus() << " " << response.getReason() << response.getKeepAlive()<< std::endl;
        char text[20] =  "ooooooooooooooooooo";
        s.getline(text,20);
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