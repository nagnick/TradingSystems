#pragma once
#include "IBroker.h"
#include "IDataPipeline.h"
class ITradingComponentFactory{
    static IBroker& getBroker(){
        return *(new IBroker());
    }
    static IDataPipeline& getDataPipeline(){
        return *(new IDataPipeline());
    }
};