#pragma once
#include <iostream>
#include <string>
class ISubscriber{ // observer according to GOF
    public:
    virtual void notify(std::string) = 0;
    virtual ~ISubscriber(){};
};