#pragma once
#include "BrokerResponse.h"

#include <string>
#include <vector>
using std::string;
class IBroker{ // classes that inherite this interface are solely for sending trades to market for execution
    // results of trade are not sent back unless requested?
    // should hold a map from orderID to hold order status 
    public:
    virtual std::vector<PositionResponse> getAllPositions() = 0; // DONE
    virtual OrderResponse cancelOrderByOrderId(string order_id) = 0; //DONE
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop)=0;  //DONE
    virtual BalanceResponse getBalance() = 0;
    virtual void getClock() = 0;
    virtual ~IBroker(){};
    };