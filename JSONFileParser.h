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
        //std::cout<< buffer.str();
        Poco::Dynamic::Var result = parser.parse(buffer.str());
        // use pointers to avoid copying
        object = result.extract<Poco::JSON::Object::Ptr>();
        // ex "{ \"test\" : { \"property\" : \"value\" } }";
        // Poco::Dynamic::Var test = object->get("test"); // holds { "property" : "value" }
        // Poco::JSON::Object::Ptr subObject = test.extract<Poco::JSON::Object::Ptr>();
        // test = subObject->get("property");
        // std::string val = test.toString(); // val holds "value"
        // copy/convert to Poco::DynamicStruct
        // Poco::DynamicStruct ds = *object;
        // val = ds["test"]["property"]; // val holds "value"
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
};