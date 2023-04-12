#pragma once

#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"
#include "PendingOrderState.h"
#include "TradingAlgos/IStateAlgo.h"

#include <string>
class BuyState: public IState{
    IStateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    enum class Direction{UP = 1, STEADY = 0,DOWN = -1};
    Direction dir;
    double lastPrice;
    bool paper;
    void calculate(double nextPrice){
        if(lastPrice == 0){
            lastPrice = nextPrice;
        }
        if(nextPrice - .1 > lastPrice){
            if(dir == Direction::STEADY){ // wasn't moving much but is now on the rise so buy
                // buy
                OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"buy","10","market","day","","");
                if(order.id != "-1"){
                    // order was accepted
                    parent->swapState(new PendingOrderState(factory,parent,symbol,paper,order.id,parent->sell,parent->buy));
                }
                // change state to pending order...
            }
            dir = Direction::UP;
        }
        else if( nextPrice - .1 < lastPrice){
            dir = Direction::DOWN;
        }
        else{
            dir = Direction::STEADY;
        }
    }
    public:
    BuyState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory), parent(_parent), symbol(_symbol), paper(_paper){
        lastPrice = 0;
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
        calculate(std::stod(quote->askPrice));
    }
    virtual ~BuyState(){

    };
};