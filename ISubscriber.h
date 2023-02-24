#pragma once

class ISubscriber{ // observer according to GOF
    public:
    virtual void subscribe() = 0;
    virtual void notify() = 0;
    virtual ~ISubscriber(){};
};