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
        this->freeCommandBuffers();
        vkDestroyPipeline(this->device->GetReadyDevice()->logicalDevice,
                            *graphicsPipeline, nullptr);
        vkDestroyCommandPool(this->device->GetReadyDevice()->logicalDevice, 
                            commandPool, nullptr);
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
    
    void vPipeline::RenderAll(uint32_t bufferIndex, uint32_t imageIndex)
    {
        this->recreateCommandBuffers(swapchain->GetImageCount());

        VkCommandBuffer& currentBuffer = this->commandBuffers[bufferIndex]; 

        this->BeginRecordCommandBuffer(bufferIndex, imageIndex);
        this->SetDynamicStates(bufferIndex);

        for(auto model : this->models)
        {
            model->bind(currentBuffer);
            model->draw(currentBuffer);
        }

        this->EndRecordCommandBuffer(bufferIndex);
    }
    void vPipeline::BeginRecordCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex)
    {
        vkResetCommandBuffer(this->commandBuffers[bufferIndex],  0);

        VkCommandBuffer& commandBuffer = this->commandBuffers[bufferIndex];
        // begin command buffer
        // > reusage of CB defined as parameters
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // bind FB to the image
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain->GetRenderPass();
        renderPassInfo.framebuffer = swapchain->GetFramebuffers()[imageIndex];
        // define size
        // should be same as imageview
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->GetExtent();

        // something like glClearColor
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        //start render pass
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // fixed state pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *graphicsPipeline);
        // dynamic state pipeline
    }
    void vPipeline::EndRecordCommandBuffer(uint32_t bufferIndex)
    {
        VkCommandBuffer commandBuffer = this->commandBuffers[bufferIndex];
        // end our render pass call
        // sth like VAO
        vkCmdEndRenderPass(commandBuffer);

        // possible other render passes(post-processing)

        // end command buffer -> chain of commands
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
    void vPipeline::SetDynamicStates(uint32_t bufferIndex)
    {
        VkCommandBuffer commandBuffer = this->commandBuffers[bufferIndex];

        auto extent = this->swapchain->GetExtent();
        this->SetWindowExtent(extent.width, extent.height);

        vkCmdSetViewport(commandBuffer, 0, 1, &this->viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &this->scissor);
    }
    void vPipeline::Initialize()
    {
        this->createCommandPools();
        this->SetScissor(0, 0);
        this->SetViewport(0.0f, 0.0f);
        this->SetDepthScissor(0.0f, 1.0f);
    }

    void vPipeline::createCommandPools()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = this->device->GetReadyDevice()->graphicsFamily.value();

        if (vkCreateCommandPool(this->device->GetReadyDevice()->logicalDevice,
                &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }
    void vPipeline::allocateCommandBuffers(uint32_t size)
    {
        commandBuffers.resize(size);


        // primary   -> submit, cannot be used from others
        // secondary -> can't submit, only use from others
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device->GetReadyDevice()->logicalDevice,
                &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
    void vPipeline::recreateCommandBuffers(uint32_t size)
    {
        if(this->commandBuffers.size() == size) return;
        this->freeCommandBuffers();
        this->allocateCommandBuffers(size);
    }
    void vPipeline::freeCommandBuffers()
    {
        if(this->commandBuffers.size() == 0) return;

        vkFreeCommandBuffers(this->device->GetReadyDevice()->logicalDevice,
            this->commandPool, 
            static_cast<uint32_t>(this->commandBuffers.size()),
            this->commandBuffers.data()
            );

        commandBuffers.clear();
    }

}