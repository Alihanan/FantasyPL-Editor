#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vDevice.h"
#include "vPipeConfig.h"
#include "vModel.h"
#include "vSwapchain.h"

#include <utility>

namespace PL
{
    class vPipeline : IUncopiable
    {
    public:
        // Others
        vPipeline(VkPipeline* pipe, vShader* shader, vDevice* device, vSwapchain* swapchain);
        ~vPipeline();
        
        //void SetModels(std::vector<std::pair<vModel*, GameModel*>>* changedModels);
        //void RenderAll(VkCommandBuffer& currentBuffer);
        void RenderModel(VkCommandBuffer& currentBuffer, vModel* model);
        vShader* GetShader() {return this->shader; }

        void SetViewport(float x, float y)
        {
            viewport.x = x;
            viewport.y = y;
        }

        void SetScissor(int32_t x_offset, int32_t y_offset)
        {       
            scissor.offset = {x_offset, y_offset};
        }
        void SetDepthScissor(float minD, float maxD)
        {
            viewport.minDepth = minD;
            viewport.maxDepth = maxD;
        }

    protected:
        VkPipeline* graphicsPipeline;
        vShader* shader;
        vDevice* device;
        vSwapchain* swapchain;
        void Initialize();        

        VkViewport viewport{};
        VkRect2D scissor{};
        void SetDynamicStates(VkCommandBuffer& commandBuffer);
        void SetWindowExtent(uint32_t w, uint32_t h)
        {
            viewport.width = w;
            viewport.height = h;   
            scissor.extent = {w, h};        
        }

       

        
    };


}