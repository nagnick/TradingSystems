#include <iostream>
#include "TradierBroker.h"
#include "TradierPipeline.h"
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
    TradierBroker broker2(file,"tradierReal");
    //broker2.getBalances();
    TradierPipeline* pipe = broker2.getPipeline();
    pipe->start();
    while(run){ // keep main thread alive until killed
    }
    pipe->stop();
    std::cout << "killed threads" << std::endl;
    return 0;
}