#pragma once

#include "IState.h"
#include "Factories/ISystemComponentFactory.h"
#include "TradingAlgos/IStateAlgo.h"

#include <string>
class BuyState: public IState{
    IStateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    enum class Direction{UP = 1, DOWN = 0};
    Direction dir;
    double lastPrice;
    bool paper;
    void calculate(double nextPrice){
        if(nextPrice - .1 > lastPrice){
            if(dir == Direction::DOWN){ // wasn't moving much but is now on the rise so buy
                // buy
                OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"buy","10","market","day","","");
                if(order.id != "-1"){
                    // order was accepted
                    parent->setOrderId(order.id);
                    parent->setLastPrice(nextPrice);
                    parent->swapToNextState();
                }
                // change state to pending order...
            }
            dir = Direction::UP;
        }
        else if( nextPrice - .1 < lastPrice){
            dir = Direction::DOWN;
        }
        lastPrice = nextPrice;
        // else dir unchanged?
    }
    public:
    BuyState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory), parent(_parent), symbol(_symbol), paper(_paper){
        lastPrice = parent->getLastPrice();
        dir = Direction::UP;
    };
    virtual void onData(std::shared_ptr<IStreamData> data){ // defualt case

    }
    virtual void onData(std::shared_ptr<OtherData> other){
        
    }
    virtual void onData(std::shared_ptr<TradeData> trade){

    }
    virtual void onData(std::shared_ptr<QuoteData> quote){
        calculate(std::stod(quote->askPrice));
    }
    virtual void init(){ // called when swapped to 
        lastPrice = parent->getLastPrice();
        dir = Direction::UP;
    }
    virtual ~BuyState(){

    };
};