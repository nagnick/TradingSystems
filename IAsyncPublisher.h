#pragma once
#include<thread>
#include<iostream>
#include "IDataPipelinePublisher.h"

class IAsyncPublisher: public IDataPipelinePublisher { // fancy subject for a subscriber (observer following GOF definition) adds a polling thread
    public:
    std::atomic_bool running;
    std::thread thread;
    IAsyncPublisher():running{false}{
    };
    virtual void loop() = 0;// left to implementer to set normally a spinning polling while running loop
    virtual void start(){ // initializes and start thread running loop
        running = true;
        thread = std::thread(&IAsyncPublisher::loop, this); // since abstract this will call subclass loop method
    }
    virtual void stop(){
        running = false;
    };
    virtual ~IAsyncPublisher(){
        running = false;
        thread.join();
    };
};