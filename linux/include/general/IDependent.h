#pragma once

#include <string>
#include <vector>
#include <set>
#include <stdexcept>

//#include <iostream>
#include <typeinfo>
// sudo apt-get install libboost-all-dev
// #include <boost/type_index.hpp>

namespace PL
{
    class IDependent
    {
    public:    
        template <class T>
        static std::string type(const T& t) {

            return demangle(typeid(t).name());
        }
        static std::string demangle(const char* name);

        

        IDependent() {}
        virtual std::string GetDependencyID() = 0;
        virtual std::vector<std::string> GetNeededDependencies() = 0;
        virtual void ReceiveContext(std::vector<std::vector<IDependent*>> context) = 0;
        virtual void UpdateContext(std::vector<std::vector<IDependent*>> context) = 0;
        virtual ~IDependent() = 0;
    protected: 
        bool isInitialized = false;
    };
}