#pragma once

#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"
#include "SellState.h"

#include <string>
class PendingOrderState: public IState{
    StateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol, orderId;
    bool paper;
    public:
    PendingOrderState(ISystemComponentFactory& _factory, StateAlgo* _parent, std::string _symbol, bool _paper, std::string _orderId):factory(_factory), parent(_parent),
        symbol(_symbol), paper(_paper), orderId(_orderId){

    };
    virtual void onData(std::shared_ptr<IStreamData> data){ // defualt case

    }
    virtual void onData(std::shared_ptr<OtherData> other){
        std::cout << " OtherData ";
        std::cout << other->data << std::endl;
    }
    virtual void onData(std::shared_ptr<TradeData> trade){

    }
    virtual void onData(std::shared_ptr<QuoteData> quote){

    }
    virtual ~PendingOrderState(){

    };
};