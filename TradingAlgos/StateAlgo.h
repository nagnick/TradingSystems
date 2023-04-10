#pragma once
#include "Factories/ISystemComponentFactory.h"
#include "States/BuyState.h"

#include <string>

class StateAlgo: public IStateAlgo{
    ISystemComponentFactory& factory;
    std::string symbol;
    bool paper;
    public:
    StateAlgo(ISystemComponentFactory& _factory, std::string symbolToTrade, bool _paper):factory(_factory),symbol(symbolToTrade), paper(_paper){
        swapState(new BuyState(factory, this, symbol, paper)); // init state
        IDataStream* stream = factory.getStream();
        // dangerous to let this escape in constructor but since the base classes are already constructed this is fine
        stream->subscribe(this);
        stream->subscribeToDataStream(symbol,this);
        start(); // start at very end to ensure the proper real methods(non abstract) are called by sub thread
    };
    virtual ~StateAlgo(){
        stop();// must kill the queued subscriber thread before continuing to destory parent classes and start queue thread in parent class
    };
};