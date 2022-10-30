#include "../../include/vulkan/vSwapchain.h"

#include <limits>
#include <algorithm>

namespace PL
{
    const std::string vSwapchain::_DEP_ID = IDependent::type(vSwapchain());

    vSwapchain::~vSwapchain()
    {
        if(!this->isInitialized) return;
        DeleteSwapchain();
        this->DeleteSynchronization();
    }

    void vSwapchain::Initialize()
    {
        this->InitializeSwapchain();
        this->InitializeSynchronization();
        this->isInitialized = true;
    }
    void vSwapchain::InitializeSwapchain()
    {
        this->CreateSwapchainKHR();
        this->CreateImages();
        this->CreateFramebuffers();
    }
    void vSwapchain::RecreateSwapchain()
    {
        this->oldSwapChain = this->swapChain;
        this->swapChain = VK_NULL_HANDLE;
        this->DeleteSwapchain();
        this->InitializeSwapchain(); 
    }
    void vSwapchain::DeleteSwapchain()
    {
        VkDevice& ldevice = this->device->GetReadyDevice()->logicalDevice;
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(ldevice, 
                                framebuffer, nullptr);
        }     
        swapChainFramebuffers.clear();

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(ldevice, imageView, nullptr);
        }
        swapChainImageViews.clear();

        if(this->swapChain != nullptr)
        {
            vkDestroySwapchainKHR(ldevice, swapChain, nullptr);
            swapChain = nullptr;
        }       
    }
    
    bool vSwapchain::AcquireImage(uint32_t* imageIndex)
    {
        
        // 0. Block 
        vkWaitForFences(device->GetReadyDevice()->logicalDevice, 
                        1, 
                        &inFlightFences[currentFrame], 
                        VK_TRUE, std::numeric_limits<uint64_t>::max());

        VkResult ret = vkAcquireNextImageKHR(this->device->GetReadyDevice()->logicalDevice, 
                            this->swapChain, std::numeric_limits<uint64_t>::max(), 
                            imageAvailableSemaphores[currentFrame], 
                            VK_NULL_HANDLE, 
                            imageIndex);
        if (ret == VK_ERROR_OUT_OF_DATE_KHR) {
            this->Recreate();
            return false;
        }
        else if (ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        return true;
    }
    bool vSwapchain::SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex)
    {
        if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device->GetReadyDevice()->logicalDevice, 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

        VkDevice& dev = this->device->GetReadyDevice()->logicalDevice;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
       
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        
        // 2. Reset
        vkResetFences(dev, 1, &inFlightFences[currentFrame]);

        VkResult ret = vkQueueSubmit(this->device->GetReadyDevice()->graphicsQueue, 
                1, &submitInfo, inFlightFences[currentFrame]);

        // 4. Submit
        if (ret != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // 5. Present
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {this->swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(this->device->GetReadyDevice()->presentQueue, &presentInfo);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        if (
                result == VK_ERROR_OUT_OF_DATE_KHR || 
                result == VK_SUBOPTIMAL_KHR ||
                this->CheckWindowResize()
            ) 
        {
            this->RecreateSwapchain();
            return false;
        } 
        else if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        return true;
    }
    void vSwapchain::InitializeSynchronization()
    {
        VkDevice& device = this->device->GetReadyDevice()->logicalDevice;

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(this->GetImageCount(), VK_NULL_HANDLE);

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

    void vSwapchain::DeleteSynchronization()
    {
        VkDevice& device = this->device->GetReadyDevice()->logicalDevice;
        
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
    }



    void vSwapchain::CreateSwapchainKHR()
    {
        auto readyDevice = this->device->GetReadyDevice();

        this->swap_presentMode = ChooseSWAPPresentMode();
        this->swap_extent = ChooseSWAPExtent();
        uint32_t imageCount = ChooseImageCount();

        // Set everything to struct
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = this->window->GetKHRSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = this->renderPass->GetSwapFormat().format;
        createInfo.imageColorSpace = this->renderPass->GetSwapFormat().colorSpace;
        createInfo.imageExtent = swap_extent;
        createInfo.imageArrayLayers = 1; // NO STEREOSCOPIC
        // NO DEPTH etc, only color attachment
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Some generic settings
        createInfo.preTransform = readyDevice->capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = swap_presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = this->oldSwapChain;

        // Queue family communication set
        uint32_t queueFamilyIndices[] = {readyDevice->graphicsFamily.value(), readyDevice->presentFamily.value()};
        
        if (readyDevice->graphicsFamily != readyDevice->presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            //createInfo.queueFamilyIndexCount = 0; // Optional
            //createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        // Create swapchain
        if (vkCreateSwapchainKHR(readyDevice->logicalDevice,
            &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
    }

    void vSwapchain::CreateImages()
    {
        auto readyDevice = this->device->GetReadyDevice();
        uint32_t imageCount;

        // Fetch images from it
        // Add Images
        vkGetSwapchainImagesKHR(readyDevice->logicalDevice, 
                                    swapChain, &imageCount, nullptr);
        this->swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(readyDevice->logicalDevice, 
                            swapChain, &imageCount, this->swapChainImages.data());
        
        // Create ImageView per Image
        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = this->renderPass->GetSwapFormat().format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(readyDevice->logicalDevice,
                         &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
        
    }

    void vSwapchain::Recreate()
    {
        vkDeviceWaitIdle(this->device->GetReadyDevice()->logicalDevice); 
        //this->window = activeWindow;
        this->oldSwapChain = this->swapChain;
        this->swapChain = VK_NULL_HANDLE;
        this->DeleteSwapchain();
        this->Initialize(); 
    }

    void vSwapchain::CreateFramebuffers()
    {
        swapChainFramebuffers.resize(this->swapChainImageViews.size());

        for (size_t i = 0; i < this->swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                this->swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = this->renderPass->GetRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = this->swap_extent.width;
            framebufferInfo.height = this->swap_extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(this->device->GetReadyDevice()->logicalDevice,
                    &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    uint32_t vSwapchain::ChooseImageCount()
    {
        auto readyDevice = this->device->GetReadyDevice();
        uint32_t imageCount = readyDevice->capabilities.minImageCount + 1;
        if (readyDevice->capabilities.maxImageCount > 0 && imageCount > readyDevice->capabilities.maxImageCount) {
            imageCount = readyDevice->capabilities.maxImageCount;
        }
        return imageCount;
    }

    
    VkPresentModeKHR vSwapchain::ChooseSWAPPresentMode()
    {
        auto& availablePresentModes = this->device->GetReadyDevice()->presentModes;

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                printf("USING MAILBOX!\n");
                return availablePresentMode;
            }
        }
        printf("USING FIFO!\n");
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D vSwapchain::ChooseSWAPExtent()
    {
        auto& capabilities = this->device->GetReadyDevice()->capabilities;
    
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = this->window->GetPixelExtent();

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    bool vSwapchain::CheckWindowResize()
    {
        
        VkExtent2D extent = this->ChooseSWAPExtent();
        bool width_eq = extent.width == this->swap_extent.width;
        bool height_eq = extent.height == this->swap_extent.height;
        return !(width_eq && height_eq);
    }
}