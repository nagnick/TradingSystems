#pragma once
#include "Brokers/IBroker.h"
#include "Streams/IDataPipeline.h"
class TradingSystemsComponentFactory{
    public:
    TradingSystemsComponentFactory(); // incase I add fields to this factory like maybe the jsonfile parser
    virtual IBroker& getBroker() = 0;
    virtual IDataPipeline& getDataPipeLine() = 0;
};