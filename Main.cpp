#include <iostream>
#include "TradierBroker.h"
#include "HTTPSClient.h"
#include "JSONFileParser.h"
#include "TradingSystemsComponentFactory.h"
#include "AlpacaBroker.h"
#include <csignal>

std::atomic_bool run = true;
void signalHandler(int i ){
    std::cout<< "Shutting down after signal: " << i << std::endl;
    run = false;
    // gracefull shutdown from a ctrl c signal....
    // clean up anything upon shutdown
}
int main(){
    std::signal(SIGINT, signalHandler);
    
    JSONFileParser file("/mnt/c/Users/nicol/Desktop/TradingSystems/broker.cfg");
    // AlpacaBroker broker(file);
    // broker.getAccount();
    // std::cout<< std::endl;

    TradierBroker broker2(file);
    broker2.getBalances();
    while(run){ // keep main thread alive until killed
    }
    return 0;
}