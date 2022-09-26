#include "../../include/vulkan/vRenderer.h"


namespace PL
{
    const std::string vRenderer::_DEP_ID = IDependent::type(vRenderer());

    vRenderer::~vRenderer()
    {
        if(!this->isInitialized) return;
        
        this->DeleteSynchronization();
    }
    
    void vRenderer::Initialize()
    {
        this->InitializeSynchronization();
        this->InitializeInfos();
    }

    void vRenderer::MainRenderTick(vWindow* activeWindow)
    {
        VkDevice& device = this->device->GetReadyDevice()->logicalDevice;
        
        

        // 0. Block 
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);  

        // 1. Acquire
        uint32_t imageIndex;
        auto result = this->swapchain->AcquireImage(&imageIndex, imageAvailableSemaphores[currentFrame]);
        presentInfo.pImageIndices = &imageIndex;

        // 1.1 Check resize
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //this->window->handleFramebufferResized();
            //this->recreateFrameOnResize();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        
        // 2. Reset
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        // 3. Record pipelines
        auto& comBufs = this->manager->RecordAllPipelines(currentFrame, imageIndex);

        submitInfo.commandBufferCount = comBufs.size();
        submitInfo.pCommandBuffers = comBufs.data();

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.pSignalSemaphores = signalSemaphores;
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};           
        submitInfo.pWaitSemaphores = waitSemaphores;
        
       
        // 4. Submit
        if (vkQueueSubmit(this->device->GetReadyDevice()->graphicsQueue, 
                1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        // 5. Present
        VkSwapchainKHR swapChains[] = {this->swapchain->GetSwapchain()};
        presentInfo.pSwapchains = swapChains;
        presentInfo.pWaitSemaphores = signalSemaphores;// wait for CB to finish

        result = vkQueuePresentKHR(this->device->GetReadyDevice()->presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
                //|| activeWindow->isFramebufferResized()
                ) 
        {
                //this->window->handleFramebufferResized();
                //this->recreateFrameOnResize();
        } 
        else if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
        // 6. Clean
        delete &comBufs;

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;        
    }

    void vRenderer::InitializeInfos()
    {
        // Submit Info
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;        
        submitInfo.waitSemaphoreCount = 1;        
        submitInfo.signalSemaphoreCount = 1;

        VkPipelineStageFlags* waitStages = new VkPipelineStageFlags[1];
        waitStages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.pWaitDstStageMask = waitStages;

        // Present Info
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.swapchainCount = 1;
        presentInfo.pResults = nullptr;
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