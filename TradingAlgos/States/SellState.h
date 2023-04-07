#pragma once

#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"

#include <string>
class SellState: public IState{
    StateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    bool paper;
    public:
    SellState(ISystemComponentFactory& _factory, StateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory), parent(_parent), symbol(_symbol), paper(_paper){

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
    virtual ~SellState(){

    };
};