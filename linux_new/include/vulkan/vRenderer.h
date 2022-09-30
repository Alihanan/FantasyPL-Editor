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
        inline const static uint32_t MAX_FRAMES_IN_FLIGHT = 2;
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
        uint32_t currentFrame = 0;

        vModelManager* manager;
        vDevice* device;
        vSwapchain* swapchain;
        void Initialize();
        
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        void InitializeSynchronization();
        void DeleteSynchronization();        

        void BeginRecordCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex);
        void EndRecordCommandBuffer(uint32_t bufferIndex);

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