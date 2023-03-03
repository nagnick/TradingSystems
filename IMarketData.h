#include <string>

class IMarketData{
    virtual std::string getDataType() = 0;
    virtual ~IMarketData(){};
}