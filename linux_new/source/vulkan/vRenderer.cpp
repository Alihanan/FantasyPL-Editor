#include "../../include/vulkan/vRenderer.h"


namespace PL
{
    const std::string vRenderer::_DEP_ID = IDependent::type(vRenderer());

    vRenderer::~vRenderer()
    {
        if(!this->isInitialized) return;
        
        this->DeleteSynchronization();
        this->freeCommandBuffers();
        vkDestroyCommandPool(this->device->GetReadyDevice()->logicalDevice, 
                            commandPool, nullptr);
    }
    
    void vRenderer::Initialize()
    {
        this->InitializeSynchronization();
        this->createCommandPools();
    }

    void vRenderer::MainRenderTick(vWindow* activeWindow)
    {
        VkDevice& device = this->device->GetReadyDevice()->logicalDevice;

        

        // 0. Block 
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);  
        // 1. Acquire
        uint32_t imageIndex;
        auto result = this->swapchain->AcquireImage(&imageIndex, imageAvailableSemaphores[currentFrame]);
        // 1.1 Check resize
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            this->ResizeSwapchain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // 2. Reset
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        // 3. Record pipelines
        this->recreateCommandBuffers(swapchain->GetImageCount());
        this->BeginRecordCommandBuffer(currentFrame, imageIndex);
        this->manager->RecordAllPipelines(this->commandBuffers[currentFrame]);
        this->EndRecordCommandBuffer(currentFrame);

        VkSubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->commandBuffers[currentFrame];

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
       
        // 4. Submit
        if (vkQueueSubmit(this->device->GetReadyDevice()->graphicsQueue, 
                1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        // 5. Present
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = {this->swapchain->GetSwapchain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(this->device->GetReadyDevice()->presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
                this->swapchain->CheckWindowResize()
                ) 
        {
            this->ResizeSwapchain();
        } 
        else if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;        
    }

    void vRenderer::ResizeSwapchain()
    {
        vkDeviceWaitIdle(this->device->GetReadyDevice()->logicalDevice); 
        this->swapchain->Recreate();
    }
    void vRenderer::CheckResizeSwapchain()
    {
        this->swapchain->CheckWindowResize();
    }


    void vRenderer::createCommandPools()
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
    void vRenderer::BeginRecordCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex)
    {
        vkResetCommandBuffer(this->commandBuffers[bufferIndex],  0);

        VkCommandBuffer commandBuffer = this->commandBuffers[bufferIndex];

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
        renderPassInfo.framebuffer = swapchain->GetActiveFramebuffer(imageIndex);
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
    }
    void vRenderer::EndRecordCommandBuffer(uint32_t bufferIndex)
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
    void vRenderer::allocateCommandBuffers(uint32_t size)
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
    void vRenderer::recreateCommandBuffers(uint32_t size)
    {
        if(this->commandBuffers.size() == size) return;
        this->freeCommandBuffers();
        this->allocateCommandBuffers(size);
    }
    void vRenderer::freeCommandBuffers()
    {
        if(this->commandBuffers.size() == 0) return;

        vkFreeCommandBuffers(this->device->GetReadyDevice()->logicalDevice,
            this->commandPool, 
            static_cast<uint32_t>(this->commandBuffers.size()),
            this->commandBuffers.data()
            );

        commandBuffers.clear();
    }

    void vRenderer::InitializeSynchronization()
    {
        VkDevice& device = this->device->GetReadyDevice()->logicalDevice;

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        // NO PARAMS NEEDED FOR EMPTY SEMAPHORES      
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        // -> start as free(signaled) -> no waiting if no previous
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void vRenderer::DeleteSynchronization()
    {
        VkDevice& device = this->device->GetReadyDevice()->logicalDevice;
        
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
    }
}