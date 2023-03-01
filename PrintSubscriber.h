#pragma once
#include "ISubscriber.h"

class PrintSubscriber: public ISubscriber{
    public:
    PrintSubscriber(){
        
    };
    virtual void notify(std::string payload){
        std:: cout <<  payload<< std::endl;
    };
};