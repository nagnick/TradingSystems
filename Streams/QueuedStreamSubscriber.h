#pragma once
#include "IDataStreamSubscriber.h"
#include "Helpers/IAsync.h"
#include "Helpers/SafeQueue.h"

class QueuedStreamSubscriber: public IAsync, public IDataStreamSubscriber{
    SafeQueue<std::shared_ptr<IStreamData>> queue;
    public:
    QueuedStreamSubscriber(){
    }
    virtual ~QueuedStreamSubscriber(){} // stop must be called in child destructor to stop pure virtual methods from being called by async thread...since
    // child destructor is called first always
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
    virtual void start(){ // call this at end of derived class constructor...
        IAsync::start();
    }
    virtual void stop(){ // must be called by child destructor to ensure proper shutdown of thread
        //IAsync::stop(); // does not work because thread join blocks...
        running = false; // i don't like this but it ensures that once the dequeue thread is relesaed by the enqueue it stops running
        queue.enqueue(std::shared_ptr<OtherData>(new OtherData("Killed queue during destruction of queued subscriber."))); // frees locks and allows thread to stop
        IAsync::stop(); // waits for thread to join 
        // weird but works enqueue frees up thread to get to running loop which is false before running again... 
    }
    // need child class to instantiate the onData methods which are called when the data is being processed off the queue(dynamic dispatch from IDataSub) by this subscribers thread
};