#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vDevice.h"

namespace PL
{
    class vMemoryManager : public IUncopiable, public IDependent
    {
    public:
        struct Buffer
        {
            VkBuffer buffer;
            VkDeviceMemory memory;
            uint32_t buf_size;

        };

        vMemoryManager() {}

        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vDevice::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->device = static_cast<vDevice*>(context[0][0]);
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
        ~vMemoryManager();
        void allocateBuffer();
        void deleteBuffer();

    protected:
        void Initialize();
        vDevice* device;


    };


}