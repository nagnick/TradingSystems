#pragma once
#include "Streams/QueuedStreamSubscriber.h"
#include "IStateAlgo.h"

#include <string>

class IStateAlgo: public QueuedStreamSubscriber{ // basic State Holder
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
    // methods for implementor to be able to control state swaps
    virtual void swapToNextState() = 0;
    virtual void swapToLastState() = 0;
    virtual void swapState(IState* newState){
        newState->init(); // call before pointer swap...
        current = newState;
    };
    virtual void setOrderId(std::string _orderId) = 0;
    virtual std::string getOrderId() = 0;
};