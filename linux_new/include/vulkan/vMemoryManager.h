#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vDevice.h"

#include <map>
#include <limits>

namespace PL
{
    class vModel;
    
    class vMemoryManager : public IUncopiable, public IDependent
    {
    public:
        struct Buffer
        {
            VkBuffer buffer;
            VkDeviceMemory memory;
            uint32_t buf_size;

        };
        struct Data
        {
            void* data;
            uint32_t size;
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

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }



        // Other members
        ~vMemoryManager();
        void AllocateVBOandUBO(vModel* model);
        VkBuffer& GetOrAllocateVBOandUBO(vModel* model);
        void FreeVBOandUBO(vModel* model);

    protected:
        void Initialize();
        Buffer* createBuffer(vModel* model);

        
        Data readData(std::string file);

        Buffer* generateBuffer(void* data, uint32_t size);
        void freeBuffer(Buffer* buf);
        vDevice* device;
        

        struct MemoryCell
        {   
            Buffer* buffer = nullptr;
            //uint32_t lastUsedTime = UINT_MAX;
        };

        uint32_t maxBufferCount = 0;
        uint32_t maxMemorySize = 0;
        //uint32_t currentTime = 0;
        std::map<vModel*, MemoryCell*> currentMemory;

    };


}