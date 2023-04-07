#pragma once

#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"

#include <string>
class BuyState: public IState{
    StateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    enum class Direction{UP = 1, STEADY = 0,DOWN = -1};
    Direction dir;
    double lastPrice;
    bool paper;
    public:
    BuyState(ISystemComponentFactory& _factory, StateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory), parent(_parent), symbol(_symbol), paper(_paper){
        dir = Direction::STEADY;
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
        double nextPrice = std::stod(quote->askPrice);
        if(nextPrice - .5 > lastPrice){
            if(dir == Direction::STEADY){ // wasn't moving much but is now on the rise so buy
                // buy
                factory.getBroker(paper)->placeEquityOrder(symbol,"buy","10","market","day","","");
                // change state to sell
            }
            dir = Direction::UP;
        }
        else if( nextPrice - .5 < lastPrice){
            dir = Direction::DOWN;
        }
        else{
            dir = Direction::STEADY;
        }
    }
    virtual ~BuyState(){

    };
};