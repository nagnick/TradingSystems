#include <string>

class IStreamData{
    virtual std::string getDataType() = 0;
    virtual ~IStreamData(){};
}