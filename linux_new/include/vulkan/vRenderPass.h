#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vDevice.h"

namespace PL
{
    class vRenderPass : public IUncopiable, public IDependent
    {
    public:
        vRenderPass() {}
        // IDependent
        inline const static std::string _DEP_ID = "vRenderPass";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vDevice::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        bool IsSingleton()
        {
            return true;
        }
        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {
            this->device = static_cast<vDevice*>(context[0][0]);
            this->Initialize();
        }


        // Other members
        ~vRenderPass();
        VkRenderPass& GetRenderPass();
        VkSurfaceFormatKHR GetSwapFormat() { return this->swap_format; }

    protected:
        vDevice* device;
        VkRenderPass renderPass;

        void Initialize();

        VkSurfaceFormatKHR swap_format;
        VkSurfaceFormatKHR ChooseSWAPFormat();
    };
}