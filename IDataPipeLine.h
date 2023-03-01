#pragma once
#include <string>
#include "ISubscriber.h"
class IDataPipeline{ // classes that inherite this interface are soley for retreving market data
    public:
    //virtual void subscribeToDataStream(std::string, ISubscriber*)=0; // too much granularity? make subscribers filter...
    virtual void subscribeToSymbolData(std::string, ISubscriber*)=0;
    virtual ~IDataPipeline(){};
};