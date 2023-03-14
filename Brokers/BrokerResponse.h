#pragma once
#include <string>

struct IBrokerResponse{
    virtual int getResponseType()=0;
    // 0 = OrderResponse; 1 = PositionResponse; 2 = BalanceResponse; 3 = ClockResponse; 4 = BarResponse;
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

struct BarResponse: public IBrokerResponse{
    std::string symbol, open, close, low, high, volume, timestamp;
    BarResponse(std::string _symbol, std::string _open, std::string _close, std::string _low,
        std::string _high, std::string _volume, std::string _timestamp):
    symbol(_symbol),open(_open),close(_close),low(_low),high(_high),volume(_volume),timestamp(_timestamp){};
    virtual int getResponseType(){
        return 4;
    };
    ~BarResponse(){};
};
