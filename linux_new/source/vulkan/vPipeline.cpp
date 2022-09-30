#include "../../include/vulkan/vPipeline.h"

#include <algorithm>


namespace PL
{
    vPipeline::vPipeline(VkPipeline* pipe, vShader* shader, vDevice* device, vSwapchain* swapchain)
        : graphicsPipeline(pipe), shader(shader), device(device), swapchain(swapchain)
    {
        this->Initialize();
    }
    vPipeline::~vPipeline()
    {
        
        vkDestroyPipeline(this->device->GetReadyDevice()->logicalDevice,
                            *graphicsPipeline, nullptr);
        
    }
    void vPipeline::AddModel(vModel* model)
    {
        if(model == nullptr) 
            throw std::runtime_error("Trying to add empty(nullptr) model to the pipeline!");
        this->models.push_back(model);
    }
    void vPipeline::DeleteModel(vModel* model)
    {
        if(model == nullptr) 
            throw std::runtime_error("Trying to delete (nullptr) from the pipeline!");

        auto position = std::find(this->models.begin(), this->models.end(), model);
        if (position != this->models.end()) // == myVector.end() means the element was not found
            this->models.erase(position);
        else
            throw std::runtime_error("Trying to delete model that is not in pipeline!");
    }
    
    void vPipeline::RenderAll(VkCommandBuffer& currentBuffer)
    {
        // fixed state pipeline
        vkCmdBindPipeline(currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *graphicsPipeline);
        // dynamic state pipeline
        this->SetDynamicStates(currentBuffer);

        for(auto model : this->models)
        {
            model->bind(currentBuffer);
            model->draw(currentBuffer);
        }

        
    }
    
    void vPipeline::SetDynamicStates(VkCommandBuffer& commandBuffer)
    {
        auto extent = this->swapchain->GetExtent();
        this->SetWindowExtent(extent.width, extent.height);

        vkCmdSetViewport(commandBuffer, 0, 1, &this->viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &this->scissor);
    }
    void vPipeline::Initialize()
    {   
        this->SetScissor(0, 0);
        this->SetViewport(0.0f, 0.0f);
        this->SetDepthScissor(0.0f, 1.0f);
    }
}