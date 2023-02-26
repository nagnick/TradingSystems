#pragma once
#include "IPublisher.h"
#include <string>
class IDataPipeline : public IPublisher{ // classes that inherite this interface are soley for retreving market data
    public:
    virtual void subscribeToDataStreams(std::string)=0;
    virtual ~IDataPipeline(){};
};