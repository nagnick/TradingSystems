#pragma once
#include "Streams/QueuedStreamSubscriber.h"
#include "States/IState.h"
#include "Factories/ISystemComponentFactory.h"
class StateAlgo; // forward declare for States to hold reference to this class
#include "States/BuyState.h"
#include "States/SellState.h"

#include <string>

class StateAlgo: public QueuedStreamSubscriber{
    IState* current = nullptr;
    ISystemComponentFactory& factory;
    std::string symbol;
    bool paper;
    public:
    StateAlgo(ISystemComponentFactory& _factory, std::string symbolToTrade, bool _paper):factory(_factory),symbol(symbolToTrade), paper(_paper){
        current = new BuyState(factory, this, symbol, paper);
        IDataStream* stream = factory.getStream();
        // dangerous to let this escape in constructor but since the base classes are already constructed this is fine
        stream->subscribe(this);
        stream->subscribeToDataStream(symbol,this);
        start(); // start at very end to ensure the proper real methods(non abstract) are called by sub thread
    };
    ~StateAlgo(){
        stop(); // must kill the queued subscriber thread before continuing to destory parent classes 
    };
    // these are called by the QueuedStreamSub when processing data. With this data let the current state decide next action
    virtual void onData(std::shared_ptr<IStreamData> data){
        current->onData(data);
    }; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other){
        current->onData(other);
    };
    virtual void onData(std::shared_ptr<TradeData> trade){
        current->onData(trade);
    };
    virtual void onData(std::shared_ptr<QuoteData> quote){
        current->onData(quote);
    };
    virtual void swapState(IState* newState){
        IState* temp = current;
        current = newState;
        delete temp;
    }
};