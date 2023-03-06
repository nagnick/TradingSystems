#pragma once
#include "IDataPipelineSubscriber.h"
class IDataPipelinePublisher { //subject for a subscriber (observer following GOF definition)
    public:
    virtual void subscribe(IDataPipelineSubscriber* subscriber) = 0;
    virtual void unSubscribe(IDataPipelineSubscriber* subscriber) = 0;
    virtual ~IDataPipelinePublisher(){};
};