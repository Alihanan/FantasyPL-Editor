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
        inline const static std::string _DEP_ID = "vDevice";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vWindow::_DEP_ID, vInstance::_DEP_ID, vPhysicalDeviceHandler::_DEP_ID
        };
        virtual std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        virtual void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->window = static_cast<vWindow*>(context[0][0]);
            this->instance = static_cast<vInstance*>(context[1][0]);
            this->physicalDeviceHandler = static_cast<vPhysicalDeviceHandler*>(context[2][0]);

            this->Initialize();
        }

        virtual void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            
        }
        
        virtual bool IsSingleton()
        {
            return true;
        }

        virtual std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        // Others
        vDevice() {}
        ~vDevice();
        std::vector<VkPhysicalDevice>& GetAvailablePhysicalDevices();
        

    protected:
        void Initialize();        

        vWindow* window;
        vInstance* instance;
        vPhysicalDeviceHandler* physicalDeviceHandler;
        vPhysicalDeviceHandler::ReadyToUseDevice* device;
    private:

        
    };
}