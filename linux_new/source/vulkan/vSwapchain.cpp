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
    }

    void vSwapchain::Initialize()
    {
        this->CreateSwapchain();
        this->CreateImages();
        this->CreateFramebuffers();
        this->isInitialized = true;
    }
    void vSwapchain::RecreateSwapchain()
    {
        this->oldSwapChain = this->swapChain;
        this->swapChain = VK_NULL_HANDLE;
        this->DeleteSwapchain();
        this->Initialize(); 
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
    
    VkResult vSwapchain::AcquireImage(uint32_t* imageIndex, VkSemaphore& sem)
    {
        VkResult ret = vkAcquireNextImageKHR(this->device->GetReadyDevice()->logicalDevice, 
                            this->swapChain, UINT64_MAX, 
                            sem, 
                            VK_NULL_HANDLE, imageIndex);
        
        return ret;
    }




    void vSwapchain::CreateSwapchain()
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
                return availablePresentMode;
            }
        }

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