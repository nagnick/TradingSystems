#pragma once
#include "IDataStreamSubscriber.h"
#include "StreamData.h"
class PrintSubscriber: public IDataStreamSubscriber{
    public:
    PrintSubscriber(){
        
    };
    virtual void notify(std::shared_ptr<IStreamData> data){
        std::cout <<  data->getDataType();
        switch(data->getDataType()){
            case -1:{
                std::cout << " OtherData ";
                std::cout << std::dynamic_pointer_cast<OtherData,IStreamData>(data)->data << std::endl;
                break;
            }
            case 0:{
                std::shared_ptr<TradeData> trade = std::dynamic_pointer_cast<TradeData,IStreamData>(data);
                std::cout << " Trade ";
                std::cout << trade->symbol << trade->exchangeCode << trade->price << trade->tradeSize << trade->time << std::endl;
                break;
            }
            case 1:{
                std::shared_ptr<QuoteData> quote = std::dynamic_pointer_cast<QuoteData,IStreamData>(data);
                std::cout << " Quote ";
                std::cout << quote->symbol << quote->bidPrice << quote->bidSize << quote->bidExchange << quote->bidTime << std::endl;
                std::cout << quote->askPrice << quote->askSize << quote->askExchange << quote->askTime <<std::endl;
                break;
            }
            default:{
                std::cout << "error unrecognized derived type of IStreamDate." << std::endl;
                break;
            }
        }
    };
};