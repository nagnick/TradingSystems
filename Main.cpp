#include <iostream>
#include "TradierBroker.h"
#include "HTTPSClient.h"
#include "JSONFileParser.h"
#include "TradingSystemsComponentFactory.h"
using namespace std;

int main(){
    cout << "Hello World\n";
    HTTPSClient client("www.google.com",443);
    std::cout<< client.connected() << std::endl;
    client.sendRequest(Poco::JSON::Object());
    std::cout<< client.connected() << std::endl;
    JSONFileParser file("/mnt/c/Users/nicol/Desktop/TradingSystems/broker.cfg");
    std::cout << file.getValue("tradier") << std::endl;
    std::cout << file.getSubObjectValue("tradier","APIKey") << std::endl;
    return 0;
}