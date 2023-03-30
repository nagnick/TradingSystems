#pragma once
#include "IDataStreamSubscriber.h"
#include "Helpers/IAsync.h"
#include "Helpers/SafeQueue.h"

class QueuedStreamSubscriber: public IAsync, public IDataStreamSubscriber{
    SafeQueue<std::shared_ptr<IStreamData>> queue;
    public:
    QueuedStreamSubscriber(){
        start(); // start thread at construction;
    }
    ~QueuedStreamSubscriber(){} // stop called when IASYNC is destroyed which is before this destructor is called
    virtual void notify(std::shared_ptr<IStreamData> data){ // Overridden to push the data to the queue
    // for queued subscriber it only gets added to its queue saves notifying time for publisher for computationally intensive subscriber TradingAlgos
        queue.enqueue(data);
    }
    virtual void loop(){
        while(running){
            IDataStreamSubscriber::notify(queue.dequeue()); // call base class traditional notify thats calls appropriate onData call
            // but only call when thread wants to(off the publisher's hot path)
        }
    }
    // these methods are called when the data is being processed off the queue(dynamic dispatch from IDataSub) by this subscribers thread default is do nothing with the data
    virtual void onData(std::shared_ptr<IStreamData> data){}; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other){};
    virtual void onData(std::shared_ptr<TradeData> trade){};
    virtual void onData(std::shared_ptr<QuoteData> quote){};
};