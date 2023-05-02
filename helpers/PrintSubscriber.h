#pragma once
#include "Streams/IDataStreamSubscriber.h"
#include "Streams/StreamData.h"
class PrintSubscriber: public IDataStreamSubscriber{
    public:
    PrintSubscriber(){};
    virtual void onData(std::shared_ptr<IStreamData> data){// defualt case
        std::cout << "Error unrecognized derived type of IStreamDate." << std::endl;
    }
    virtual void onData(std::shared_ptr<OtherData> other){
        std::cout << "OtherData: ";
        std::cout << other->data << std::endl;
    }
    virtual void onData(std::shared_ptr<TradeData> trade){
        std::cout << "Trade: ";
        std::cout << "Symbol: " <<trade->symbol << " Exchange: " << trade->exchangeCode << " Price: " << trade->price << " Size: " << trade->tradeSize << " Time: " << trade->time << std::endl;
    }
    virtual void onData(std::shared_ptr<QuoteData> quote){
        std::cout << "Quote: ";
        std::cout << "Symbol: "<< quote->symbol << " Bid Price: " << quote->bidPrice << " Bid Size: " << quote->bidSize << " Bid Exchange: " << quote->bidExchange << " Bid Time: " << quote->bidTime << "\n";
        std::cout << "\tAsk Price: " << quote->askPrice << "Ask Size: " << quote->askSize << " Ask Exchange: " << quote->askExchange << " Ask Time: " << quote->askTime <<std::endl;
    }
};