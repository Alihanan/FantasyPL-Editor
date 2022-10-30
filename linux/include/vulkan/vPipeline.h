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
        void RenderAll(VkCommandBuffer& currentBuffer);
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

        std::vector<vModel*> models;
        

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