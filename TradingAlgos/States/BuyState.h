#pragma once

#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"

#include <string>
class BuyState: public IState{
    StateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    public:
    BuyState(ISystemComponentFactory& _factory, StateAlgo* _parent, std::string _symbol):factory(_factory), parent(_parent), symbol(_symbol){

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
    virtual ~BuyState(){

    };
};