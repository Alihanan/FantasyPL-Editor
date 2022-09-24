#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vDevice.h"
#include "vPipeConfig.h"
#include "vModel.h"
#include "vSwapchain.h"

namespace PL
{
    class vPipeline : IUncopiable
    {
    public:
        // Others
        vPipeline(VkPipeline* pipe, vShader* shader, vDevice* device, vSwapchain* swapchain);
        ~vPipeline();
        
        void AddModel(vModel* model);
        void DeleteModel(vModel* model);
        void RenderAll(uint32_t bufferIndex, uint32_t imageIndex);
        vShader* GetShader() {return this->shader; }
    protected:
        VkPipeline* graphicsPipeline;
        vShader* shader;
        vDevice* device;
        vSwapchain* swapchain;
        void Initialize();

        std::vector<vModel*> models;
        void BeginRecordCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex);
        void EndRecordCommandBuffer(uint32_t bufferIndex);

        VkCommandPool commandPool;
        void createCommandPools();

        std::vector<VkCommandBuffer> commandBuffers;
        void allocateCommandBuffers(uint32_t size);
        void recreateCommandBuffers(uint32_t size);
        void freeCommandBuffers();
    };


}