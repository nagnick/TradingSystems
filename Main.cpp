
#include "Factories/AlpacaSystemComponentFactory.h"
#include "Factories/TradierSystemComponentFactory.h"
#include "Helpers/PrintSubscriber.h"
#include "Helpers/JSONFileParser.h"
#include "TradingAlgos/SimpleAlgo.h"
#include "TradingAlgos/StateAlgo.h"
#include "TradingAlgos/IChainAlgo.h"
#include <stdlib.h>
#include <signal.h>
#include <iostream>

std::atomic_bool run = true;
void signalHandler(int i ){
    std::cout<< "Shutting down after signal: " << i << std::endl;
    run = false;
    // gracefull shutdown from a ctrl c signal....
    // clean up anything upon shutdown
}
int main(){
    signal(SIGINT, signalHandler);
    TradierSystemComponentFactory tFactory;
    AlpacaSystemComponentFactory aFactory;
    bool paperTrading = true; //true == paper trading 
    IBroker* brokerT = tFactory.getBroker(paperTrading);
    IBroker* brokerA = aFactory.getBroker(paperTrading);
    
    // PrintSubscriber sub;
    IDataStream* pipeA = aFactory.getStream();
    //SimpleAlgo strat(aFactory,"SPY",paperTrading);
    //IChainAlgo front;
    try{
    StateAlgo states(aFactory,"SPY",paperTrading);
    states.subscribeToStream(); // if stand alone without chain
    }
    catch(const Poco::Exception e){
        std::cout << e.displayText() << e.message() << e.what() << std::endl;
    }
    //front.swapNextInChain(&states);
    //pipeA->subscribe(&states);
    //pipeA->subscribeToDataStream("SPY",&states);
    //test crypto
     PrintSubscriber print;
    // IDataStream* pipeA = aFactory.getCryptoStream();
    // StateAlgo states(aFactory,"BTC/USD",paperTrading);
     pipeA->subscribe(&print);
     pipeA->subscribeToDataStream("SPY",&print);
    //pipeA->subscribe(&states);
    //pipeA->subscribeToDataStream("BTC/USD",&states);

    while(run){ // keep main thread alive until killed
    sleep(600);
    }
    std::cout << "killed threads" << std::endl;
    return 0;
}