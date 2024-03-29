#pragma once
#include<thread>
#include<iostream>

class IAsync{ // adds a polling thread
    public:
    std::atomic_bool running;
    std::thread thread;
    IAsync():running{false}{
    };
    virtual void loop() = 0;// left to implementer to set normally a spinning polling while running loop
    virtual void start(){ // initializes and start thread running loop
        running = true;
        thread = std::thread(&IAsync::loop, this); // since abstract this will call subclass loop method
    }
    virtual void stop(){ // need to call in inherited destructors to prevent pure virtual methods from being called during destruction
        running = false;
        thread.join();
    };
    virtual ~IAsync(){
        if(running){
            stop();
        }
    };
};