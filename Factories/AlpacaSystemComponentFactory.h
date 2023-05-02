#pragma once
#include "ISystemComponentFactory.h"
#include "Streams/AlpacaStream.h"
#include "Brokers/AlpacaBroker.h"
#include "Streams/AlpacaCryptoStream.h"

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
        static IDataStream* stream = (IDataStream*)new AlpacaStream(*getConfigFile(),"alpacaReal", "/v2/iex", 443); //"/v2/iex" = exchange with free data not great but free
        // protect streams from creating other instances without being a singleton (maintain one pub to many subs model)
        return stream;
    };
    virtual ICryptoBroker* getCryptoBroker(bool paper){ // upto user to free
        if(paper){
            return new AlpacaBroker(*getConfigFile(), "alpacaPaper"); // returns new instance each time to give each user a clean connection to the broker
        }
        return new AlpacaBroker(*getConfigFile(), "alpacaReal");
    };
    virtual IDataStream* getCryptoStream(){// factory will free
        static IDataStream* stream = (IDataStream*)new AlpacaCryptoStream(*getConfigFile(),"alpacaReal", "/v1beta3/crypto/us", 443);
        // protect streams from creating other instances without being a singleton (maintain one pub to many subs model)
        return stream;
    };
    virtual ~AlpacaSystemComponentFactory(){};
};