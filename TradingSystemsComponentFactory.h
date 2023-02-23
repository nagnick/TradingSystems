#pragma once
#include "IBroker.h"
#include "IDataPipeLine.h"
class TradingSystemsComponentFactory{
    public:
    TradingSystemsComponentFactory(); // incase I add fields to this factory
    virtual IBroker& getBroker() = 0;
    virtual IDataPipeLine& getDataPipeLine() = 0;
};