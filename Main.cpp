
#include "Brokers/TradierBroker.h"
#include "Brokers/AlpacaBroker.h"
#include "Streams/TradierStream.h"
#include "Streams/AlpacaStream.h"
#include "Streams/PrintSubscriber.h"
#include "Helpers/JSONFileParser.h"
#include "TradingAlgos/SimpleStrategy.h"

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
    TradierBroker brokerT(file,"tradierPaper");
    AlpacaBroker brokerA(file, "alpacaPaper");
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
    AlpacaStream* pipeA = new AlpacaStream(file,"alpacaReal", "/v2/iex", 443);
    //SimpleStrategy strat(brokerA, *pipeA);
    // std::string sessionId = brokerT.getWebsocketSessionId();
    // TradierStream* pipeT = new TradierStream(file,"tradierReal", sessionId, "/v1/markets/events", 443);
    pipeA->start();
    pipeA->subscribe(&sub);
    pipeA->subscribeToDataStream("SPY",&sub);
    while(run){ // keep main thread alive until killed
    }
    //pipeA->stop(); //not required when pipeA is destructed the tread is killed
    std::cout << "killed threads" << std::endl;
    return 0;
}