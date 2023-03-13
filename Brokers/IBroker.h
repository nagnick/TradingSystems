#pragma once
#include "BrokerResponse.h"

#include <string>
#include <vector>
using std::string;
class IBroker{ // classes that inherite this interface are solely for sending trades to market for execution
    // just a start;  these are all I should need to start making trading algorithms 
    // the two brokers may still need formating in the responses to have them work the same
    public:
    virtual std::vector<PositionResponse> getAllPositions() = 0; // DONE
    virtual OrderResponse cancelOrderByOrderId(string order_id) = 0; //DONE
    virtual OrderResponse placeEquityOrder(string symbol, string side, string quantity, string type,
        string duration, string price, string stop)=0;  //DONE
    virtual BalanceResponse getBalance() = 0; // DONE
    virtual ClockResponse getClock() = 0; // DONE
    virtual ~IBroker(){};
    };