#include "Streams/IDataStreamSubscriber.h"
#include "Brokers/IBroker.h"

#include "vector"
class SimpleStrategy: public IDataStreamSubscriber {
    IBroker& broker;
    IDataStream& stream;
    double open, close, high, low, volume;
    double buyingPower; // tends to include margin....
    double entryPrice = 0;
    double exitPrice = 1000;
    bool inPosition = false;
    public:
    SimpleStrategy(IBroker& _broker, IDataStream& _stream):broker(_broker),stream(_stream){
        stream.subscribe(this);
        stream.subscribeToDataStream("SPY",this);
        std::vector<BarResponse> bars = broker.getDailyHistoricalBars("SPY","","");
        buyingPower = std::stod(broker.getBalance().buying_power);
        //std::cout << "Buying power" << buyingPower << std::endl;
        if(bars.size() > 0){ // initialize the strats market stats...
            BarResponse bar = bars.at(bars.size()-1);
            open = std::stod(bar.open);
            close = std::stod(bar.close);
            high = std::stod(bar.high);
            low = std::stod(bar.low);
            volume = std::stod(bar.volume);
        }
        // std::cout << "size of bars:" << bars.size() << std::endl;
        // for (auto &&bar : bars){
        //     std::cout << "time stamp" << bar.timestamp << std::endl;
        // }
    };
    virtual void notify(std::shared_ptr<IStreamData> data){
        //std::cout <<  data->getDataType();
        switch(data->getDataType()){
            case -1:{
                //std::cout << " OtherData ";
                //std::cout << std::dynamic_pointer_cast<OtherData,IStreamData>(data)->data << std::endl;
                break;
            }
            case 0:{
                std::shared_ptr<TradeData> trade = std::dynamic_pointer_cast<TradeData,IStreamData>(data);
                // std::cout << " Trade ";
                // std::cout << trade->symbol << trade->exchangeCode << trade->price << trade->tradeSize << trade->time << std::endl;
                break;
            }
            case 1:{
                std::shared_ptr<QuoteData> quote = std::dynamic_pointer_cast<QuoteData,IStreamData>(data);
                // std::cout << " Quote ";
                // std::cout << quote->symbol << quote->bidPrice << quote->bidSize << quote->bidExchange << quote->bidTime << std::endl;
                // std::cout << quote->askPrice << quote->askSize << quote->askExchange << quote->askTime <<std::endl;
                double priceIcanBuyAt = std::stod(quote->askPrice); // ask price = lowest a seller will sell at
                double priceIcanSellAt = std::stod(quote->bidPrice); // bid price = highest a buyer will buy at
                if(!inPosition && priceIcanBuyAt < open && priceIcanBuyAt < exitPrice){
                    //buy
                    entryPrice = priceIcanBuyAt;
                    OrderResponse res = broker.placeEquityOrder("SPY","buy","10","market","day","","");
                    std::cout << res.status << res.id << std::endl;
                    inPosition = true;
                    std::cout << "Bought 10 shares of spy" << std::endl;
                }
                if(inPosition && priceIcanSellAt > entryPrice){
                    // sell
                    exitPrice = priceIcanSellAt;
                    OrderResponse res = broker.placeEquityOrder("SPY","sell","10","market","day","","");
                    std::cout << res.status << res.id << std::endl;
                    inPosition = false;
                    std::cout << "Sold 10 shares of spy" << std::endl;
                }
                break;
            }
            default:{
                //std::cout << "error unrecognized derived type of IStreamDate." << std::endl;
                break;
            }
        }
    };
    ~SimpleStrategy(){

    };
};