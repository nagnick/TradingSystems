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
    double lastPrice;
    void completedRounds(){
        if(rounds > 20){
            IBroker* broker = factory.getBroker(paper);
            OrderResponse res = broker->getOrderByOrderId(orderId);
            //std::cout << res.status;
            if(res.status == "filled"){
                parent->setLastPrice(lastPrice);
                parent->swapToNextState();
            }
            else if(res.status == "partially_filled" || res.status == "new"){
                rounds= 0;
                // else{ // something is wrong order is not getting filled
                //     res = broker->cancelOrderByOrderId(orderId);
                //      std::cout << res.status << " OrderID: " << orderId << std::endl;
                //     if(res.status == "ok"){ // restart in previous state
                //         parent->swapToLastState();
                //     }
                // }
                return;
            }
            else{ // order has not yet been filled something went wrong...
                res = broker->cancelOrderByOrderId(orderId);
                if(res.status == "ok"){ // restart in previous state
                    parent->setLastPrice(lastPrice);
                    parent->swapToLastState();
                }
            }
        }
    }
    public:
    PendingOrderState(ISystemComponentFactory& _factory, IStateAlgo* _parent, std::string _symbol, bool _paper):factory(_factory),
        parent(_parent), symbol(_symbol), paper(_paper){
            lastPrice = parent->getLastPrice();
    };
    void setOrderIdToWatch(std::string _orderId){
        orderId = _orderId;
    }
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
        lastPrice = std::stod(quote->askPrice);
        completedRounds();
    }
    virtual ~PendingOrderState(){

    };
    virtual void init(){ // called when swapped to 
        rounds = 0;
        lastPrice = parent->getLastPrice();
        setOrderIdToWatch(parent->getOrderId());
    }
};