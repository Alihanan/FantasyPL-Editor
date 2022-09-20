#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vDevice.h"
#include "vPipeConfig.h"

namespace PL
{
    class vPipeline : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vDevice::_DEP_ID, vPipeConfig::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            
            this->Initialize();
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            auto ret = std::string(typeid(this).name());
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }
        
        // Others
        vPipeline() {}
        ~vPipeline();

    protected:
        void Initialize();
        
    };


}