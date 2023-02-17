#include "IBroker.h"
class ITradingComponentFactory{
    static IBroker& getBroker(){
        return *(new IBroker());
    }
};