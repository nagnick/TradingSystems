// this class will handle all network connectivity that involves HTTP
#pragma once
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
class HTTPClient{ // pocoClientseccion wrapper
    Poco::Net::HTTPClientSession* session;
    public:
    HTTPClient(){
        Poco::URI uri("http://pocoproject.org/images/front_banner.jpg");
        session = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
    };
    void setKeepAlive(bool alive){
        session->setKeepAlive(alive);
    };
    void sendRequest(Poco::Net::HTTPRequest& request){
        session->sendRequest(request);
    };
};