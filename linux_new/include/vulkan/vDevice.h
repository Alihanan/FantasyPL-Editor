#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vPhysicalDeviceHandler.h"
#include "vWindow.h"
#include "vInstance.h"

#include <optional>

namespace PL
{
    class vDevice : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        const static std::string _DEP_ID;
        // inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
        //     vWindow::_DEP_ID, vInstance::_DEP_ID, vPhysicalDeviceHandler::_DEP_ID
        // };
        std::vector<std::string> GetNeededDependencies()
        {
            return { vWindow::_DEP_ID, vInstance::_DEP_ID, vPhysicalDeviceHandler::_DEP_ID};
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->window = static_cast<vWindow*>(context[0][0]);
            this->instance = static_cast<vInstance*>(context[1][0]);
            this->physicalDeviceHandler = static_cast<vPhysicalDeviceHandler*>(context[2][0]);

            this->Initialize();
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        // Others
        vDevice() {}
        ~vDevice();
        std::vector<VkPhysicalDevice>& GetAvailablePhysicalDevices();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        vPhysicalDeviceHandler::ReadyToUseDevice* GetReadyDevice();

    protected:
        void Initialize();        

        vWindow* window;
        vInstance* instance;
        vPhysicalDeviceHandler* physicalDeviceHandler;
        vPhysicalDeviceHandler::ReadyToUseDevice* device;
    private:

        
    };
}