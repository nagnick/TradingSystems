#pragma once
#include "BrokerResponse.h"
#include <string>
using std::string;
class ICryptoBroker{
    public:
    // the min required for now....
    virtual OrderResponse placeCryptoOrder(string symbol, string side, string quantity, string type,
        string duration, string limitPrice, string stopPrice)=0;  //DONE
    virtual OrderResponse cancelOrderByOrderId(string order_id) = 0; //DONE
    virtual OrderResponse getOrderByOrderId(string order_id) = 0; // DONE
    virtual ~ICryptoBroker(){};
};