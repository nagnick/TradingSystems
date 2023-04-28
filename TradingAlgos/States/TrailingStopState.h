#pragma once
#include "IState.h"

class TrailingStopState: public IState{
    public:
    TrailingStopState(){};
    virtual ~TrailingStopState(){};
    virtual void init(){};
    virtual void onData(std::shared_ptr<IStreamData> data){}; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other){};
    virtual void onData(std::shared_ptr<TradeData> trade){};
    virtual void onData(std::shared_ptr<QuoteData> quote){};
};