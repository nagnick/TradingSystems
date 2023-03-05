#pragma once
#include <string>
struct IStreamData{ // base class for all data that can be returned by a dataPipeline.
    virtual int getDataType() = 0;
    /* Other = -1, Trade = 0, Quote = 1*/
    virtual ~IStreamData(){};
};
struct TradeData : public IStreamData{
    // type = 0 always to rep trade
    std::string symbol, exchangeCode, price, tradeSize, time; // these are common accross both
    std::string tradeId, cumulativeVolume, tradeConditon, tape; // unique to one or the other. Also tradier has last price but is dup of price so not included. can probably be dropped.
    TradeData(std::string _symbol, std::string _exchangeCode, std::string _price, std::string _tradeSize, std::string _time):
    symbol(_symbol),exchangeCode(_exchangeCode),price(_price),tradeSize(_tradeSize),time(_time){
        tradeId = "";
        cumulativeVolume = "";
        tradeConditon = "";
        tape = "";
    };
    virtual int getDataType(){
        return 0;
    };
    virtual ~TradeData(){};
};
struct QuoteData: public IStreamData{
    //type = 1 always to rep quote
    std::string symbol, bidPrice, bidSize, bidExchange, bidTime, askPrice, askSize, askExchange, askTime; //alpaca uses single time tradier has both a bid & ask time
    std::string tradeSize, quoteCondition, tape; // only apply to alpaca can probably be dropped.
    QuoteData(std::string _symbol, std::string _bidPrice, std::string _bidSize, std::string _bidExchange, std::string _bidTime, std::string _askPrice, 
        std::string _askSize, std::string _askExchange, std::string _askTime): symbol(_symbol), bidPrice(_bidPrice), bidSize(_bidSize),
        bidExchange(_bidExchange), bidTime(_bidTime), askPrice(_askPrice), askSize(_askSize), askExchange(_askExchange),askTime(_askTime){
        tradeSize = "";
        quoteCondition = "";
        tape = "";
    }
    virtual int getDataType(){
        return 1;
    };
    virtual ~QuoteData(){};
};
struct OtherData: public IStreamData{
    std::string data;
    OtherData(std::string _data):data(_data){};
    virtual int getDataType(){
        return -1;
    };
    virtual ~OtherData(){};
};