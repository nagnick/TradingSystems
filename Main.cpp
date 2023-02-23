#include <iostream>
#include "TradierBroker.h"
#include "HTTPSClient.h"
#include "JSONFileParser.h"
#include "TradingSystemsComponentFactory.h"
#include "AlpacaBroker.h"
using namespace std;

int main(){
    JSONFileParser file("/mnt/c/Users/nicol/Desktop/TradingSystems/broker.cfg");
    AlpacaBroker broker(file);
    //HTTPSClient client(file.getSubObjectValue("alpaca","URL") ,443,file.getSubObjectValue("alpaca","AuthScheme"),file.getSubObjectValue("alpaca","AuthInfo"));
    //std::cout<< client.connected() << std::endl;
    //client.sendRequest(Poco::JSON::Object(),"GET","/v2/account");
    //std::cout<< client.connected() << std::endl;
    broker.sendRequest("GET","/v2/account",Poco::JSON::Object());
    return 0;
}