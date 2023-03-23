#pragma once
#include "StreamData.h"

#include <iostream>
#include <string>
#include <memory>
class IDataStreamSubscriber{ // observer according to GOF
    public:
    virtual void notify(std::shared_ptr<IStreamData> data){ // can be overridden but not required to saves duplicate code
        switch(data->getDataType()){
            case -1:{
                onData(std::dynamic_pointer_cast<OtherData,IStreamData>(data));
                break;
            }
            case 0:{
                onData(std::dynamic_pointer_cast<TradeData,IStreamData>(data));
                break;
            }
            case 1:{
                onData(std::dynamic_pointer_cast<QuoteData,IStreamData>(data));
                break;
            }
            default:{
                onData(data);
                break;
            }
        }
    }
    // these methods are called when the data is sent to this subscriber from a stream
    virtual void onData(std::shared_ptr<IStreamData> data) = 0; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other) = 0;
    virtual void onData(std::shared_ptr<TradeData> trade) = 0;
    virtual void onData(std::shared_ptr<QuoteData> quote) = 0;
    virtual ~IDataStreamSubscriber(){};
};