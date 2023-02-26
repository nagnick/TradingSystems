#pragma once
#include "IBroker.h"
#include "IDataPipeline.h"
class TradingSystemsComponentFactory{
    public:
    TradingSystemsComponentFactory(); // incase I add fields to this factory
    virtual IBroker& getBroker() = 0;
    virtual IDataPipeline& getDataPipeLine() = 0;
};