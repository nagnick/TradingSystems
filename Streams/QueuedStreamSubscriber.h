#pragma once
#include "IDataStreamSubscriber.h"
#include "Helpers/IAsync.h"

class QueuedStreamSubscriber: public IDataStreamSubscriber, public IAsync{
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    front(){
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        return queue_.front();
    }
    public:
    virtual void notify(std::shared_ptr<IStreamData> data){ // can be overridden but not required to saves duplicate code
    // for queued subscriber it only gets added to its queue saves notifying time for publisher for computationally intensive subscriber strategies
    
    }
    virtual void process(std::shared_ptr<IStreamData> data){ // swap to do traditional notify casting but only call when thread wants to(off the publisher's hot path)
        switch(data->getDataType()){
            case -1:{
                onData(std::dynamic_pointer_cast<OtherData,IStreamData>(data));
                break;
            }
            case 0:{
                onData(std::dynamic_pointer_cast<TradeData,IStreamData>(data));
                break;
            }
            case 1:{
                onData(std::dynamic_pointer_cast<QuoteData,IStreamData>(data));
                break;
            }
            default:{
                onData(data);
                break;
            }
        }
    }
    // these methods are called when the data is being processed off the queue by this subscribers thread
    virtual void onData(std::shared_ptr<IStreamData> data) = 0; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other) = 0;
    virtual void onData(std::shared_ptr<TradeData> trade) = 0;
    virtual void onData(std::shared_ptr<QuoteData> quote) = 0;
}