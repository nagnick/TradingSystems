#pragma once 
#include "IDataPipeline.h"
#include "IAsyncPublisher.h"
class AlpacaPipeline : public IDataPipeline, public IAsyncPublisher {
    public:
    AlpacaPipeline(){

    };
    virtual void subscribeToSymbolData(std::string, ISubscriber*){

    };
    virtual void subscribe(ISubscriber* subscriber){

    };
    virtual void unSubscribe(ISubscriber* subscriber){

    };
    virtual void loop(){
        while(running){

        }
    };
    virtual void start(){
        running = true;
        thread = std::thread(&AlpacaPipeline::loop, this);
    };
    ~AlpacaPipeline(){
        stop();
    }
};