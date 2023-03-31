#pragma once
#include "Helpers/JSONFileParser.h"
#include "Brokers/IBroker.h"
#include "Streams/IDataStream.h"

class ISystemComponentFactory{
    JSONFileParser* jsonFile = nullptr;
    public:
    ISystemComponentFactory(){
        jsonFile = new JSONFileParser("/mnt/c/Users/nicol/Desktop/TradingSystems/broker.cfg");
    }
    virtual JSONFileParser* getConfigFile(){ // factory will free. it always returns same instance to save time
        return jsonFile;
    }
    virtual IBroker* getBroker(bool paper) = 0;
    virtual IDataStream* getStream() = 0;
    virtual ~ISystemComponentFactory(){
        delete jsonFile;
    };
};
