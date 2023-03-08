#pragma once

struct IBrokerResponse{
    virtual int getType()=0;
    ~IBrokerResponse(){};
};
