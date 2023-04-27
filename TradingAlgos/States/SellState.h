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
    enum class Direction{UP = 1,DOWN = -1};
    Direction dir;
    double lastPrice;
    void calculate(double nextPrice){
        if(nextPrice - .1 < lastPrice){
                // sell was going up but now going down....
                if(dir == Direction::UP){
                    OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"sell","10","market","day","","");
                    if(order.id != "-1"){
                        // order was accepted
                        parent->setLastPrice(nextPrice);
                        parent->setOrderId(order.id);
                        parent->swapToNextState();
                    // change state to pending order...
                    }
                }
            dir = Direction::DOWN;
        }
        else if( nextPrice - .1 > lastPrice){
            dir = Direction::UP;
        }
    }
    public:
    SellState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory), parent(_parent), symbol(_symbol), paper(_paper){
        lastPrice = parent->getLastPrice();
        dir = Direction::DOWN;
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
        lastPrice = parent->getLastPrice();
        dir = Direction::DOWN;
    }
    virtual ~SellState(){

    };
};