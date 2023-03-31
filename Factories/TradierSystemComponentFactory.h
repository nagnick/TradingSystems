#pragma once
#include "ISystemComponentFactory.h"
#include "Streams/TradierStream.h"
#include "Brokers/TradierBroker.h"

class TradierSystemComponentFactory: public ISystemComponentFactory{
    public:
    TradierSystemComponentFactory(){
    }
    virtual IBroker* getBroker(bool paper){ // upto user to free
        if(paper){
            return new TradierBroker(*(this->getConfigFile()),"tradierPaper");// returns new instance each time to give each user a clean connection to the broker
        }
        return new TradierBroker(*(this->getConfigFile()),"tradierReal");
    };
    virtual IDataStream* getStream(){ // factory will free
        static IDataStream* stream = (IDataStream*)new TradierStream(*getConfigFile(), (TradierBroker&)*getBroker(false),"tradierReal","/v1/markets/events", 443);
        // protect streams from creating other instances without being a singleton (maintain one pub to many subs model)
        return stream;
    };
    virtual ~TradierSystemComponentFactory(){};
};