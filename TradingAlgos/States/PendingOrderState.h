#pragma once
#include "Brokers/BrokerResponse.h"
#include "IState.h"
#include "TradingAlgos/StateAlgo.h"
#include "Factories/ISystemComponentFactory.h"
#include "TradingAlgos/IStateAlgo.h"

#include <string>
class PendingOrderState: public IState{
    IStateAlgo* parent;
    ISystemComponentFactory& factory;
    std::string symbol, orderId;
    bool paper;
    int rounds = 0;
    bool second = false;
    IState* nextState = nullptr;
    IState* previousState = nullptr;
    void completedRounds(){
        if(rounds > 10){
            IBroker* broker = factory.getBroker(paper);
            OrderResponse res = broker->getOrderByOrderId(orderId);
            if(res.status == "filled"){
                parent->swapState(nextState);
            }
            else if( res.status == "partially_filled"){
                if(!second){// go for a second 10 rounds
                    second = true;
                    rounds = 0;
                }
                else{ // something is wrong order is not getting filled
                    res = broker->cancelOrderByOrderId(orderId);
                    if(res.status == "ok"){ // restart in previous state
                        parent->swapState(previousState);
                    }
                }
            }
            else{ // order has not yet been filled something went wrong...
                res = broker->cancelOrderByOrderId(orderId);
                if(res.status == "ok"){ // restart in previous state
                    parent->swapState(previousState);
                }
            }
        }
    }
    public:
    PendingOrderState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper, std::string _orderId, IState* _nextState, IState* _previousState):factory(_factory),
        parent(_parent), symbol(_symbol), paper(_paper), orderId(_orderId), nextState(_nextState), previousState(_previousState){

    };
    virtual void onData(std::shared_ptr<IStreamData> data){ // defualt case
        rounds++;
        completedRounds();
    }
    virtual void onData(std::shared_ptr<OtherData> other){
        std::cout << " OtherData ";
        std::cout << other->data << std::endl;
        rounds++;
        completedRounds();
    }
    virtual void onData(std::shared_ptr<TradeData> trade){
        rounds++;
        completedRounds();
    }
    virtual void onData(std::shared_ptr<QuoteData> quote){
        rounds++;
        completedRounds();
    }
    virtual ~PendingOrderState(){

    };
};