#pragma once
#include <string>
#include "IDataPipelineSubscriber.h"
class IDataPipeline{ // classes that inherite this interface are soley for retreving market data
    public:
    virtual void subscribeToDataStream(std::string, IDataPipelineSubscriber*)=0;
    virtual ~IDataPipeline(){};
};