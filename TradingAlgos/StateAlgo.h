#pragma once
#include "Streams/QueuedStreamSubscriber.h"
#include "States/IState.h"
class StateAlgo: public QueuedStreamSubscriber{
    IState current;
    public:
    StateAlgo(){

    };
    ~StateAlgo(){};
    // these are called by the QueuedStreamSub when processing data. With this data decide what to do...buy sell hold
    virtual void onData(std::shared_ptr<IStreamData> data){}; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other){};
    virtual void onData(std::shared_ptr<TradeData> trade){};
    virtual void onData(std::shared_ptr<QuoteData> quote){};
};