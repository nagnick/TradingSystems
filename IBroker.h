#pragma once
#include <string>
class IBroker{ // classes that inherite this interface are solely for sending trades to market for execution
    // results of trade are not sent back unless requested?
    // should hold a map from orderID to hold order status 
    public:
    //virtual void submitOrder() = 0; // order builder??
    //virtual void orderStatus() = 0;
    virtual ~IBroker(){};
    };