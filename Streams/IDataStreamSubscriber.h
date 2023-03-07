#pragma once
#include "StreamData.h"

#include <iostream>
#include <string>
#include <memory>
class IDataStreamSubscriber{ // observer according to GOF
    public:
    virtual void notify(std::shared_ptr<IStreamData> data) = 0;
    virtual ~IDataStreamSubscriber(){};
};