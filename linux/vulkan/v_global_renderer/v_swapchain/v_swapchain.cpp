#include "v_swapchain.h"
#include <algorithm>

namespace PL
{
    v_swapchain* v_swapchain::currentSwapchain = nullptr;

    void v_swapchain::createV_swapchain()
    {
        if(v_swapchain::currentSwapchain == nullptr)
        {
            v_swapchain::currentSwapchain = new v_swapchain();
        }
    }
    void v_swapchain::recreateSwapchain()
    {
        if(v_swapchain::currentSwapchain == nullptr)
        {
            throw std::runtime_error("trying to access swapchain before creation");
        }
        v_swapchain::currentSwapchain->recreate();
    }
    VkResult v_swapchain::acquireNextImage(uint32_t* imageIndex, VkSemaphore& semaphore)
    {
        if(v_swapchain::currentSwapchain == nullptr)
        {
            throw std::runtime_error("trying to access swapchain before creation");
        }
        return v_swapchain::currentSwapchain->acquireImg(imageIndex, semaphore);
    }
    VkResult v_swapchain::presentResult(std::vector<VkCommandBuffer>& bufs, VkSemaphore& semaphore)
    {
        if(v_swapchain::currentSwapchain == nullptr)
        {
            throw std::runtime_error("trying to access swapchain before creation");
        }
        return v_swapchain::currentSwapchain->present(bufs, semaphore);
    }
    void v_swapchain::cleanUp()
    {
        if(v_swapchain::currentSwapchain != nullptr)
        {
            delete v_swapchain::currentSwapchain;
        }
    }
    /*
     * END OF STATIC PART
    */

    v_swapchain::v_swapchain()
    {
       this->init();     
    }

    void v_swapchain::init()
    {
        this->initializeSwapchain();
        this->initiailizeImageViews();
        this->createRenderpass();
        this->createFramebuffers();
    }

    v_swapchain::~v_swapchain()
    {     
        this->deleteSwapchain();    
        delete this->renderpass;
        this->renderpass == nullptr;
    }

    VkResult v_swapchain::acquireImg(uint32_t* imageIndex, VkSemaphore& semaphore)
    {
        VkDevice& device = v_device::LDevice(DEVICE_MAIN);
        VkResult result = vkAcquireNextImageKHR(v_device::LDevice(DEVICE_MAIN), 
                this->swapChain, UINT64_MAX, 
                semaphore, 
                VK_NULL_HANDLE, imageIndex);
        return result;
    }
    
    VkResult v_swapchain::present(std::vector<VkCommandBuffer>& bufs, VkSemaphore& semaphore)
    {
        VkDevice& device = v_device::LDevice(DEVICE_MAIN);
    }

    void v_swapchain::initializeSwapchain()
    {
        // Get all settings
        auto swapChainSupport = v_device::querySwapchainSupport(DEVICE_MAIN);

        this->surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        
        // Set everything to struct
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = v_window::getSurface(WINDOW_MAIN);
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // NO STEREOSCOPIC
        // NO DEPTH etc, only color attachment
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // SOME QUEUE FAMILIES
        QueueFamilyIndices indices = v_device::getAvailableQueueFamily(DEVICE_MAIN);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        // Some generic settings
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = this->oldSwapChain;
        
        // Create
        if (vkCreateSwapchainKHR(v_device::LDevice(DEVICE_MAIN),
            &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
        
        // Add Images
        vkGetSwapchainImagesKHR(v_device::LDevice(DEVICE_MAIN), 
                                    swapChain, &imageCount, nullptr);
        this->swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(v_device::LDevice(DEVICE_MAIN), 
                            swapChain, &imageCount, this->swapChainImages.data());
        
        this->swapChainImageFormat = surfaceFormat.format;
        this->swapChainExtent = extent;
    }

    void v_swapchain::deleteSwapchain()
    {
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(v_device::LDevice(DEVICE_MAIN), imageView, nullptr);
        }
        swapChainImageViews.clear();

        if(this->swapChain != nullptr)
        {
            vkDestroySwapchainKHR(v_device::LDevice(DEVICE_MAIN), swapChain, nullptr);
            swapChain = nullptr;
        }       
        
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(v_device::LDevice(DEVICE_MAIN), 
                                framebuffer, nullptr);
        }     
        swapChainFramebuffers.clear();
        
        //delete this->renderpass;
    }

    void v_swapchain::createRenderpass()
    {
        if(this->renderpass == nullptr)
        {
            this->renderpass = new v_renderpass(this->getSwapchainImageFormat());
        }
    }

    void v_swapchain::recreate(VkExtent2D windowExtent, VkExtent2D pixelExtent)
    {
        this->oldSwapChain = this->swapChain;
        this->swapChain = VK_NULL_HANDLE;
        this->deleteSwapchain();
        this->init(); 
    }

    void v_swapchain::createFramebuffers()
    {
        swapChainFramebuffers.resize(this->swapChainImageViews.size());

        for (size_t i = 0; i < this->swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                this->swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderpass->getRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = this->swapChainExtent.width;
            framebufferInfo.height = this->swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(v_device::LDevice(DEVICE_MAIN),
                    &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void v_swapchain::initiailizeImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(v_device::LDevice(DEVICE_MAIN),
                         &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }
    
    VkSurfaceFormatKHR v_swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR v_swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D v_swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = v_window::getPixelExtent(WINDOW_MAIN);

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

}