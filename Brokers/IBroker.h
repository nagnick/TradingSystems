#pragma once
#include "BrokerResponse.h"

#include <string>
#include <vector>
using std::string;
class IBroker{ // classes that inherite this interface are solely for sending trades to market for execution
    // results of trade are not sent back unless requested?
    // should hold a map from orderID to hold order status 
    public:
    //virtual void submitOrder() = 0;
    //virtual void orderStatus() = 0;
    virtual std::vector<PositionResponse> getAllPositions() = 0;
    virtual void cancelOrderByOrderId(string order_id) = 0;
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop)=0; // order builder?? could know which representation to return and required fields so I don't have to make tons of overloaded methods
    virtual void getClock() = 0;
    virtual ~IBroker(){};
    };