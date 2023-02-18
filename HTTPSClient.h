// this class will handle all network connectivity that involves HTTPS
#pragma once
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
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
    void sendRequest(std::string requestBody){
        std::string version = "1.1";
        Poco::Net::HTTPRequest request(version);
        request.setHost(url, port);
        request.add("Authorization", "Bearer"); // add token to request??
        request.setContentType("application/json");
        session->sendRequest(request);
    };
};