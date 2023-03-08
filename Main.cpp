
#include "Brokers/TradierBroker.h"
#include "Brokers/AlpacaBroker.h"
#include "Streams/TradierStream.h"
#include "Streams/AlpacaStream.h"
#include "Streams/PrintSubscriber.h"
#include "helpers/JSONFileParser.h"
#include <csignal>
#include <iostream>

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
    AlpacaBroker brokerA(file, "alpacaReal");
    //broker2.getBalances();
    PrintSubscriber sub;
    AlpacaStream* pipeA = new AlpacaStream(file,"alpacaReal", "/v2/iex", 443);
    std::string sessionId = brokerT.getWebsocketSessionId();
    TradierStream* pipeT = new TradierStream(file,"tradierReal", sessionId, "/v1/markets/events", 443);
    pipeA->start();
    pipeA->subscribe(&sub);
    pipeA->subscribeToDataStream("SPY",&sub);
    while(run){ // keep main thread alive until killed
    }
    pipeA->stop();
    std::cout << "killed threads" << std::endl;
    return 0;
}