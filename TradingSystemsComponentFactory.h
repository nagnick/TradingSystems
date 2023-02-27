#pragma once
#include "IBroker.h"
#include "IDataPipeline.h"
class TradingSystemsComponentFactory{
    public:
    TradingSystemsComponentFactory(); // incase I add fields to this factory like maybe the jsonfile parser
    virtual IBroker& getBroker() = 0;
    virtual IDataPipeline& getDataPipeLine() = 0;
};