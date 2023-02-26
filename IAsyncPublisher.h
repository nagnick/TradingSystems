#pragma once
#include<thread>
#include<iostream>
#include "IPublisher.h"

class IAsyncPublisher: public IPublisher { // fancy subject for a subscriber (observer following GOF definition) adds a polling thread
    public:
    std::atomic_bool running;
    std::thread thread;
    IAsyncPublisher():running{false}{
    };
    virtual void loop() = 0;// left to implementer to set normally a spinning polling loop
    virtual void start(){ // initializes and start thread running loop
        running = true;
        thread = std::thread(&IAsyncPublisher::loop, this);
    }
    virtual void stop(){
        running = false;
    };
    virtual ~IAsyncPublisher(){
        running = false;
        thread.join();
    };
};