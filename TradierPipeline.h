#pragma once
#include "IDataPipeline.h"
#include "IAsyncPublisher.h"
class TradierPipeline: public IDataPipeline, IAsyncPublisher{

    public:
    TradierPipeline(){
    }
    virtual void loop(){
        while(running){// a spinning polling loop

        }
    };
    virtual void start(){
        running = true;
        thread = std::thread(&TradierPipeline::loop, this);
    }
    virtual void subscribeToDataStreams(std::string){};
    virtual void subscribe(ISubscriber* subscriber) {};
    virtual void unSubscribe(ISubscriber* subscriber){};
    virtual ~TradierPipeline(){
        stop();
    };
};