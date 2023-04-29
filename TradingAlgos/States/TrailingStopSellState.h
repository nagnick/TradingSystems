#pragma once
#include "IState.h"
#include "Factories/ISystemComponentFactory.h"
#include "TradingAlgos/IStateAlgo.h"

class TrailingStopState: public IState{ // my own implementation of trailing stops
    IStateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol;
    double lastPrice;
    bool paper;
    bool percentStop;
    double stop;
    double highWaterMark;
    enum class Direction{UP = 1, DOWN = 0};
    Direction dir;
    void calculate(double nextPrice){
        if(highWaterMark < nextPrice){
            highWaterMark = nextPrice;
        }
        if(nextPrice - .1 < lastPrice){
                // sell was going up but now going down....
                if(dir == Direction::UP){
                    double lowWaterMark = highWaterMark;
                    if(percentStop){
                        lowWaterMark -= highWaterMark * stop;
                    }else{ // must be dollar value drop
                        lowWaterMark -= stop;
                    }
                    if(nextPrice <= lowWaterMark){
                        OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"sell","10","market","day","","");
                        if(order.id != "-1"){
                            // order was accepted
                            parent->setLastPrice(nextPrice);
                            parent->setOrderId(order.id);
                            parent->swapToNextState();
                        // change state to pending order...
                        }
                    }
                }
            dir = Direction::DOWN;
        }
        else if( nextPrice - .1 > lastPrice){
            dir = Direction::UP;
        }
        lastPrice = nextPrice;
    }
    public:
    TrailingStopState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper, bool _percentStop, double _stop):factory(_factory),
     parent(_parent), symbol(_symbol), paper(_paper), percentStop(_percentStop), stop(_stop){
    };
    virtual ~TrailingStopState(){};
    virtual void init(){
        highWaterMark = 0;
        lastPrice = parent->getLastPrice();
        dir = Direction::DOWN;
    };
    virtual void onData(std::shared_ptr<IStreamData> data){}; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other){};
    virtual void onData(std::shared_ptr<TradeData> trade){};
    virtual void onData(std::shared_ptr<QuoteData> quote){
        calculate(std::stod(quote->askPrice));
    };
};