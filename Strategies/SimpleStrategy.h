#include "Streams/IDataStreamSubscriber.h"
#include "Brokers/IBroker.h"

#include "vector"
class SimpleStrategy: public IDataStreamSubscriber {
    IBroker& broker;
    IDataStream& stream;
    std::string open, close, high, low, volume;
    std::string buyingPower; // tends to include margin....
    public:
    SimpleStrategy(IBroker& _broker, IDataStream& _stream):broker(_broker),stream(_stream){
        stream.subscribe(this);
        stream.subscribeToDataStream("SPY",this);
        std::vector<BarResponse> bars = broker.getDailyHistoricalBars("SPY","","");
        buyingPower = broker.getBalance().buying_power;
        //std::cout << "Buying power" << buyingPower << std::endl;
        if(bars.size() > 0){ // initialize the strats market stats...
            BarResponse bar = bars.at(bars.size()-1);
            open = bar.open;
            close = bar.close;
            high = bar.high;
            low = bar.low;
            volume = bar.volume;
        }
        // std::cout << "size of bars:" << bars.size() << std::endl;
        // for (auto &&bar : bars){
        //     std::cout << "time stamp" << bar.timestamp << std::endl;
        // }
    };
    virtual void notify(std::shared_ptr<IStreamData> data){
        std::cout <<  data->getDataType();
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
                std::cout << " Quote ";
                std::cout << quote->symbol << quote->bidPrice << quote->bidSize << quote->bidExchange << quote->bidTime << std::endl;
                std::cout << quote->askPrice << quote->askSize << quote->askExchange << quote->askTime <<std::endl;
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