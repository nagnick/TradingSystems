#pragma once
#include "Factories/ISystemComponentFactory.h"
#include "States/BuyState.h"
#include "States/SellState.h"
#include "States/TrailingStopSellState.h"
#include "States/PendingOrderState.h"
#include <string>
#include <vector>

class StateAlgo: public IStateAlgo{ //vector based state holder
    ISystemComponentFactory& factory;
    std::string symbol;
    bool paper;
    int currentState;
    std::vector<IState*> states;
    std::string orderId;
    double lastPrice;
    public:
    StateAlgo(ISystemComponentFactory& _factory, std::string symbolToTrade, bool _paper):factory(_factory),symbol(symbolToTrade), paper(_paper){
        IState* buy = new BuyState(factory, this, symbol, paper);
        IState* sell = new TrailingStopState(factory, this, symbol, paper, false, 1.00); // does dollar drop check
        //new SellState(factory, this, symbol, paper); old replaced by trailing stop loss
        IState* pending = new PendingOrderState(factory,this,symbol,paper);
        // init state cycle order
        states.push_back(buy);
        states.push_back(pending);
        states.push_back(sell);
        states.push_back(pending);
        // init current/start state
        swapState(buy);
        currentState = 0;
        //set last price var
        std::vector<BarResponse> bars = factory.getBroker(paper)->getDailyHistoricalBars(symbolToTrade,"","");
        if(bars.size() > 0){
            lastPrice = std::stod(bars.at(0).open);
            std::cout << "open price: " << lastPrice << std::endl;
        }
        //IDataStream* stream = factory.getStream();
        // dangerous to let this escape in constructor but since the base classes are already constructed this is fine
        //stream->subscribe(this);
        //stream->subscribeToDataStream(symbol,this);
        start(); // start at very end to ensure the proper real methods(non abstract) are called by sub thread
    };
    void subscribeToStream(){ // call this if algo is stand alone and must subscribe itself
        IDataStream* stream = factory.getStream();
        stream->subscribe(this);
        stream->subscribeToDataStream(symbol,this);
    }
    void swapToNextState(){
        currentState++;
        if(currentState >= states.size()){
            currentState = 0; // wrap to front
        }
        swapState(states.at(currentState));
    }
    void swapToLastState(){
        currentState--;
        if(currentState < 0){
            currentState = states.size()-1; // wrap to end
        }
        swapState(states.at(currentState));
    }
    virtual ~StateAlgo(){
        stop();// must kill the queued subscriber thread before continuing to destory parent classes and start queue thread in parent class
    };
    void setOrderId(std::string _orderId){ // update whenever a new order is placed
        orderId = _orderId;
    }
    std::string getOrderId(){
        return orderId;
    }
    void setLastPrice(double _lastPrice){ // update whenever a new order is placed
        lastPrice = _lastPrice;
    }
    double getLastPrice(){
        return lastPrice;
    }
};