#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vSwapchain.h"

namespace PL
{
    class vRenderer : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        inline const static std::string _DEP_ID = "vRenderer";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vSwapchain::_DEP_ID
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
            
        }
        bool IsSingleton()
        {
            return true;
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }
        
        // Other members
        ~vRenderer();
        void MainRenderTick();

    protected:
        void Initialize();
    };


}