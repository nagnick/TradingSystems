
#include "Factories/AlpacaSystemComponentFactory.h"
#include "Factories/TradierSystemComponentFactory.h"
#include "Streams/PrintSubscriber.h"
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
    //JSONFileParser file("/mnt/c/Users/nicol/Desktop/TradingSystems/broker.cfg");
    bool paperTrading = true;
    IBroker* brokerT = tFactory.getBroker(paperTrading);
    IBroker* brokerA = aFactory.getBroker(paperTrading);
    // std::cout << "tradier:"<< std::endl;
    // std::vector<BarResponse> temp = brokerT.getDailyHistoricalBars("SPY","2023-03-04","");
    // for(int i = 0; i < temp.size(); i++){
    //     BarResponse bar = temp.at(i);
    //     std::cout << bar.low << " " << bar.high << " " << bar.open << " " << bar.close << " " << bar.volume << " " << bar.timestamp << std::endl;
    // }
    // std::cout << "alpaca:"<< std::endl;
    // temp = brokerA.getDailyHistoricalBars("SPY","2023-03-04","");
    // for(int i = 0; i < temp.size(); i++){
    //     BarResponse bar = temp.at(i);
    //     std::cout << bar.low << " " << bar.high << " " << bar.open << " " << bar.close << " " << bar.volume << " " << bar.timestamp << std::endl;
    // }
    // ClockResponse temp = brokerT.getClock();
    // std::cout << "tradier: ";
    // std::cout << "time: "<< temp.timestamp << " market is open: " << temp.is_open << std::endl; 
    // temp = brokerA.getClock();
    // std::cout << "alpaca: ";
    // std::cout << "time: "<< temp.timestamp << " market is open: " << temp.is_open << std::endl; 
    // std::vector<PositionResponse> result = brokerA.getAllPositions();
    // std::cout << result.size() << std::endl;
    // OrderResponse res = brokerT.placeEquityOrder("AAPL","buy","10","limit","day","100","NULL");
    // std::cout << "response type = " << res.getResponseType() << " orderID:"<<res.id << " status:" << res.status << std::endl;
    // OrderResponse res = brokerT.cancelOrderByOrderId("5740682");
    // std::cout << "response type = " << res.getResponseType() << " orderID:"<<res.id << " status:" << res.status << std::endl;
    //OrderResponse res = brokerT.placeEquityOrder("AAPL","sell","10","market","day","NULL","NULL");
    //std::cout << "response type = " << res.getResponseType() << " orderID:"<<res.id << " status:" << res.status << std::endl;
    // result = brokerT.getAllPositions();
    // std::cout << result.size() << std::endl;
    // OrderResponse res = brokerT.cancelOrderByOrderId("5718856");
    // std::cout << "response type = " << res.getResponseType() << " orderID:"<<res.id << " status:" << res.status << std::endl;
    //broker2.getBalances();
    PrintSubscriber sub;
    IDataStream* pipeA = aFactory.getStream();
    //SimpleAlgo strat(brokerA, *pipeA);
    // std::string sessionId = brokerT.getWebsocketSessionId();
    //IDataStream* pipeT = tFactory.getStream();
    IChainAlgo front;
    StateAlgo states(aFactory,"SPY",true); // during construction will subscribe to stream from factory passed in  true == paper trading 
    //states.subscribeToStream();
    front.swapNextInChain(&states);
    pipeA->subscribe(&front);
    pipeA->subscribeToDataStream("SPY",&front);
    while(run){ // keep main thread alive until killed
    sleep(600);
    }
    std::cout << "killed threads" << std::endl;
    return 0;
}