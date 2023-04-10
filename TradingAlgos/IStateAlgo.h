#pragma once
#include "Streams/QueuedStreamSubscriber.h"
#include "IStateAlgo.h"

#include <string>

class IStateAlgo: public QueuedStreamSubscriber{
    IState* current = nullptr;
    public:
    IState* buy = nullptr;
    IState* sell = nullptr;
    virtual ~IStateAlgo(){
        if( buy != current){
            delete buy;
        }
        if(sell != current){
            delete sell;
        }
        delete current;
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
        if( buy != current && sell != current){
            delete current;
        }

        current = newState;
    };
};