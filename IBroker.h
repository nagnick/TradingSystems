#pragma once
#include <string>
class IBroker{ // classes that inherite this interface are solely for sending trades to market for execution
    // results of trade are not sent back unless requested?
    // should hold a map from orderID to hold order status 
    public:
    IBroker(){

    };
    //virtual void submitOrder(); // order builder??
    //virtual void orderStatus(); 
};