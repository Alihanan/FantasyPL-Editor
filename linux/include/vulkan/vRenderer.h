#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vSwapchain.h"
#include "vDevice.h"
#include "vModelManager.h"

namespace PL
{
    class vRenderer : public IUncopiable, public IDependent
    {
    public:
        
        // IDependent
        const static std::string _DEP_ID;
        // inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
        //     vSwapchain::_DEP_ID
        // };
        std::vector<std::string> GetNeededDependencies()
        {
            return {vDevice::_DEP_ID, vSwapchain::_DEP_ID, vModelManager::_DEP_ID};
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {                    
            this->device = static_cast<vDevice*>(context[0][0]);
            this->swapchain = static_cast<vSwapchain*>(context[1][0]);
            this->manager = static_cast<vModelManager*>(context[2][0]);
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
        ~vRenderer();
        void MainRenderTick(vWindow* activeWindow);

    protected:
        vModelManager* manager;
        vDevice* device;
        vSwapchain* swapchain;
        void Initialize();
        
        

        void BeginRecordCommandBuffer(uint32_t imageIndex);
        void EndRecordCommandBuffer(uint32_t imageIndex);

        uint32_t currentFrameIndex = 0;
        VkCommandPool commandPool;
        void createCommandPools();
        std::vector<VkCommandBuffer> commandBuffers;
        void allocateCommandBuffers(uint32_t size);
        void recreateCommandBuffers(uint32_t size);
        void freeCommandBuffers();

        VkRenderPassBeginInfo renderPassInfo{};

        void ResizeSwapchain();
        void CheckResizeSwapchain();
    };


}