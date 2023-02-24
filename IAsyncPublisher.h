#pragma once
#include<thread>
#include<iostream>
#include "IPublisher.h"

class IAsyncPublisher: public IPublisher { // fancy subject for a subscriber (observer following GOF definition) adds a polling thread
    public:
    std::atomic_bool running;
    std::thread thread;
    IAsyncPublisher():running{true}{
        
    };
    virtual void start() = 0;// left to implementer to set normally a spinnig polling loop
    virtual void stop(){
        running = false;
    };
    virtual ~IAsyncPublisher(){
        running = false;
        thread.join();
    };
};