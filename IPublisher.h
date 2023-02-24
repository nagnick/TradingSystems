#pragma once
#include "ISubscriber.h"
class IPublisher { //subject for a subscriber (observer following GOF definition)
    public:
    virtual void subscribe(ISubscriber* subscriber) = 0;
    virtual void unSubscribe(ISubscriber* subscriber) = 0;
    virtual ~IPublisher(){};
};