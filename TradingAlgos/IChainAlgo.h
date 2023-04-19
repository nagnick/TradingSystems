#pragma once
#include "Streams/QueuedStreamSubscriber.h"

class IChainAlgo: public QueuedStreamSubscriber{ // chain of responsibility trading algo interface...
    IDataStreamSubscriber* nextInChain; // accepts both queued and non queued subs thanks to poly
    public:
    IChainAlgo():nextInChain{}{
        start(); // start queue thread
    }
    virtual void swapNextInChain(IDataStreamSubscriber* next){
        nextInChain = next;
    }
    // default is to just forward the request
    virtual void onData(std::shared_ptr<IStreamData> data){
        if(nextInChain){
            nextInChain->onData(data);
        }
    }
    virtual void onData(std::shared_ptr<OtherData> other){
        if(nextInChain){
            nextInChain->onData(other);
        }
    }
    virtual void onData(std::shared_ptr<TradeData> trade){
        if(nextInChain){
            nextInChain->onData(trade);
        }
    }
    virtual void onData(std::shared_ptr<QuoteData> quote){
        if(nextInChain){
            nextInChain->onData(quote);
        }
    }
    virtual ~IChainAlgo(){
        nextInChain = nullptr; // prevent it calling killed objects during destruction of chain
        stop();// must kill the queued subscriber thread before continuing to destory parent classes and start queue thread in parent class
    }
};