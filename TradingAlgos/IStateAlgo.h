#pragma once
#include "Streams/QueuedStreamSubscriber.h"
#include "IStateAlgo.h"

#include <string>

class IStateAlgo: public QueuedStreamSubscriber{
    IState* current = nullptr;
    public:
    virtual ~IStateAlgo(){};
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