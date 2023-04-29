#pragma once
#include "Streams/IDataStreamSubscriber.h"
#include "Streams/StreamData.h"
class PrintSubscriber: public IDataStreamSubscriber{
    public:
    PrintSubscriber(){};
    virtual void onData(std::shared_ptr<IStreamData> data){// defualt case
        std::cout << "error unrecognized derived type of IStreamDate." << std::endl;
    }
    virtual void onData(std::shared_ptr<OtherData> other){
        std::cout << " OtherData ";
        std::cout << other->data << std::endl;
    }
    virtual void onData(std::shared_ptr<TradeData> trade){
        std::cout << " Trade ";
        std::cout << trade->symbol << trade->exchangeCode << trade->price << trade->tradeSize << trade->time << std::endl;
    }
    virtual void onData(std::shared_ptr<QuoteData> quote){
        std::cout << " Quote ";
        std::cout << quote->symbol << quote->bidPrice << quote->bidSize << quote->bidExchange << quote->bidTime << "\n";
        std::cout << quote->askPrice << quote->askSize << quote->askExchange << quote->askTime <<std::endl;
    }
};