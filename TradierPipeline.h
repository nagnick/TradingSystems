#pragma once
#include "IDataPipeline.h"
class TradierPipeline: public IDataPipeline{
    
    public:
    virtual void subscribeToDataStreams(std::string)=0;
    virtual void subscribe(ISubscriber* subscriber) = 0;
    virtual void unSubscribe(ISubscriber* subscriber) = 0;
    virtual ~TradierPipeline(){};
};