#pragma once
#include <fstream>
#include <string>
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
class JSONFileParser{
    Poco::JSON::Object::Ptr object;
    public:
    JSONFileParser(std::string fileName){
        std::ifstream t(fileName);
        std::stringstream buffer;
        buffer << t.rdbuf();
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(buffer.str());
        object = result.extract<Poco::JSON::Object::Ptr>();
    }
    std::string getValue(std::string key){
        Poco::Dynamic::Var test = object->get(key);
        return test.toString();
    }
    std::string getSubObjectValue(std::string subObjectKey, std::string key){
        Poco::Dynamic::Var test = object->get(subObjectKey);
        Poco::JSON::Object::Ptr subObject = test.extract<Poco::JSON::Object::Ptr>();
        test = subObject->get(key);
        return test.toString();
    }
    ~JSONFileParser(){

    }
};