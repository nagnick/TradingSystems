#pragma once
#include <string>
class IDataPipeline{ // classes that inherite this interface are soley for retreving market data
    public:
    virtual void subscribeToDataStreams(std::string)=0;
    virtual ~IDataPipeline(){};
};