#pragma once
#include <string>

struct IBrokerResponse{
    virtual int getResponseType()=0;
    // 0 = OrderResponse; 1 = PositionResponse; 2 = BalanceResponse; 3 = ClockResponse;
    ~IBrokerResponse(){};
};
struct OrderResponse: public IBrokerResponse{
    std::string id, status; // lots of other stuff by alpaca but probably unessesary 
    OrderResponse(std::string _id, std::string _status):id(_id),status(_status){};
    virtual int getResponseType(){
        return 0;
    };
    ~OrderResponse(){};
};
struct PositionResponse: public IBrokerResponse{
    std::string symbol, quantity, cost_basis, id; // lots of other stuff but probably unessesary
    // check for alpaca quantity as tradier goes negative for short but alpaca has a short field 
    PositionResponse(std::string _symbol, std::string _quantity, std::string _cost_basis, std::string _id):
    symbol(_symbol),quantity(_quantity),cost_basis(_cost_basis),id(_id){};
    virtual int getResponseType(){
        return 1;
    };
    ~PositionResponse(){};
};
struct BalanceResponse: public IBrokerResponse{
    std::string cash, buying_power;
    BalanceResponse(std::string _cash, std::string _buying_power):cash(_cash),buying_power(_buying_power){};
    virtual int getResponseType(){
        return 2;
    };
    ~BalanceResponse(){};
};
struct ClockResponse: public IBrokerResponse{
    std::string timestamp;
    bool is_open;
    ClockResponse(std::string _timestamp, bool _is_open):timestamp(_timestamp),is_open(_is_open){};
    virtual int getResponseType(){
        return 3;
    };
    ~ClockResponse(){};
};
