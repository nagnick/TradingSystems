#pragma once
#include "ISystemComponentFactory.h"
#include "Streams/AlpacaStream.h"
#include "Brokers/AlpacaBroker.h"
class AlpacaSystemComponentFactory: public ISystemComponentFactory{
    public:
    AlpacaSystemComponentFactory(){
    }
    virtual IBroker* getBroker(bool paper){ // upto user to free
        if(paper){
            return new AlpacaBroker(*getConfigFile(), "alpacaPaper"); // returns new instance each time to give each user a clean connection to the broker
        }
        return new AlpacaBroker(*getConfigFile(), "alpacaReal");
    };
    virtual IDataStream* getStream(){// factory will free
        static IDataStream* stream = (IDataStream*)new AlpacaStream(*getConfigFile(),"alpacaReal", "/v2/iex", 443);
        // protect streams from creating other instances without being a singleton (maintain one pub to many subs model)
        return stream;
    };
    virtual ~AlpacaSystemComponentFactory(){};
};