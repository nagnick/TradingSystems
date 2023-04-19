#pragma once

#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"
#include "TradingAlgos/IStateAlgo.h"

#include <string>
class SellState: public IState{
    IStateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    bool paper;
    enum class Direction{UP = 1, STEADY = 0,DOWN = -1};
    Direction dir;
    double lastPrice;
    void calculate(double nextPrice){
        if(lastPrice == 0){
            lastPrice = nextPrice;
        }
        if(nextPrice - .1 < lastPrice){
            if(dir == Direction::STEADY){ // wasn't moving much but is now on the drop so sell
                // sell
                OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"sell","10","market","day","","");
                if(order.id != "-1"){
                    // order was accepted
                    parent->setOrderId(order.id);
                    parent->swapToNextState();
                }
                // change state to pending order...
            }
            dir = Direction::DOWN;
        }
        else if( nextPrice - .1 > lastPrice){
            dir = Direction::UP;
        }
        else{
            dir = Direction::STEADY;
        }
    }
    public:
    SellState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory), parent(_parent), symbol(_symbol), paper(_paper){
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
    virtual void init(){ // called when swapped to 

    }
    virtual ~SellState(){

    };
};