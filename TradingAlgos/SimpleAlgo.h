#pragma once
#include "Factories/ISystemComponentFactory.h"

#include "vector"
class SimpleAlgo: public IDataStreamSubscriber {
    ISystemComponentFactory& factory;
    std::string symbol;
    bool paper;
    std::string orderId;
    double lastPrice;
    enum class States{BUY,SELL,PENDING};
    States currentState = States::BUY;
    enum class Direction{UP,DOWN};
    Direction currentDirtection = Direction::UP;
    public:
    SimpleAlgo(ISystemComponentFactory& _factory, std::string _symbol, bool _paper):factory(_factory), symbol(_symbol), paper(_paper){
        IDataStream* stream = factory.getStream();
        stream->subscribe(this);
        stream->subscribeToDataStream(_symbol,this);
        std::vector<BarResponse> bars = factory.getBroker(paper)->getDailyHistoricalBars(_symbol,"","");
        if(bars.size() > 0){ // initialize the strats market stats...
            BarResponse bar = bars.at(bars.size()-1);
            lastPrice = std::stod(bar.open);
            // close = std::stod(bar.close);
            // high = std::stod(bar.high);
            // low = std::stod(bar.low);
            // volume = std::stod(bar.volume);
        }
    };
    virtual void onData(std::shared_ptr<IStreamData> data){
        // default case do nothing with is fall through data
    }
    virtual void onData(std::shared_ptr<OtherData> other){
        // do nothing
    }
    virtual void onData(std::shared_ptr<TradeData> trade){
        // do nothing
    }
    virtual void onData(std::shared_ptr<QuoteData> quote){
        double nextPrice = std::stod(quote->askPrice);
        switch(currentState){
            case States::BUY:{
                if(nextPrice - .1 > lastPrice){
                    if(currentDirtection == Direction::DOWN){ // wasn't moving much but is now on the rise so buy
                        // buy
                        OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"buy","10","market","day","","");
                        if(order.id != "-1"){
                            // order was accepted
                            orderId = order.id;
                            currentState = States::PENDING;
                        }
                        // change state to pending order...
                    }
                    currentDirtection = Direction::UP;
                }
                else if( nextPrice - .1 < lastPrice){
                    currentDirtection = Direction::DOWN;
                }
                break;
            }
            case States::PENDING:{
                IBroker* broker = factory.getBroker(paper);
                OrderResponse res = broker->getOrderByOrderId(orderId);
                if(res.status == "filled"){
                    currentState = States::SELL;
                }
                else if(res.status == "partially_filled" || res.status == "new"){
                    return;
                }
                else{ // order has not yet been filled something went wrong...
                    res = broker->cancelOrderByOrderId(orderId);
                    if(res.status == "ok"){ // restart in previous state
                        currentState = States::BUY;
                    }
                }
                break;
            }
            case States::SELL:{
                if(nextPrice - .1 < lastPrice){
                    // sell was going up but now going down....
                    if(currentDirtection == Direction::UP){
                        OrderResponse order = factory.getBroker(paper)->placeEquityOrder(symbol,"sell","10","market","day","","");
                        if(order.id != "-1"){
                            // order was accepted
                            orderId = order.id;
                            currentState = States::BUY;
                        // change state to pending order...
                        }
                    }
                    currentDirtection = Direction::DOWN;
                }
                else if( nextPrice - .1 > lastPrice){
                    currentDirtection = Direction::UP;
                }
                break;
            }
        }
        lastPrice = nextPrice;
    }
    ~SimpleAlgo(){};
};