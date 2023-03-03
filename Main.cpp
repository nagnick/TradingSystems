#include <iostream>
#include "TradierBroker.h"
#include "TradierPipeline.h"
#include "HTTPSClient.h"
#include "PrintSubscriber.h"
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
    TradierBroker brokerT(file,"tradierReal");
    AlpacaBroker brokerA(file, "alpaca");
    //broker2.getBalances();
    PrintSubscriber sub;
    // /v2/{source} iex or sip to {source} iex is all you get without paying for subscription 
    AlpacaPipeline* pipeA = new AlpacaPipeline(file,"alpaca", "/v2/iex", 443);
    std::string sessionId = brokerT.getWebsocketSessionId();
    TradierPipeline* pipeT = new TradierPipeline(file,"tradierReal", sessionId, "/v1/markets/events", 443);
    pipeT->start();
    pipeT->subscribe(&sub);
    pipeT->subscribeToSymbolData("SPY",&sub);
    while(run){ // keep main thread alive until killed
    }
    pipeT->stop();
    std::cout << "killed threads" << std::endl;
    return 0;
}