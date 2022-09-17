#pragma once

#include <string>
#include <vector>
#include <set>
#include <stdexcept>

namespace PL
{
    
    class IDependent
    {
    public:       
        IDependent() {}
        virtual std::string GetDependencyID() = 0;
        virtual std::vector<std::string> GetNeededDependencies() = 0;
        virtual bool IsSingleton() = 0;
        virtual void ReceiveContext(std::vector<std::vector<IDependent*>> context) = 0;
        virtual void UpdateContext(std::vector<std::vector<IDependent*>> context) = 0;
        virtual ~IDependent() = 0;
    };
}