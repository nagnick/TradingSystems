#pragma once
#include <string>
#include "IDataStreamSubscriber.h"
class IDataStream{ // classes that inherite this interface are soley for retreving market data are a built in subject for subscribers (observers GOF)
    public:
    virtual void subscribe(IDataStreamSubscriber* subscriber) = 0;
    virtual void unSubscribe(IDataStreamSubscriber* subscriber) = 0;
    virtual void subscribeToDataStream(std::string, IDataStreamSubscriber*)=0;
    virtual ~IDataStream(){};
};