#include "../../include/vulkan/vSwapchain.h"

#include <limits>
#include <algorithm>

namespace PL
{
    vSwapchain::~vSwapchain()
    {

    }

    void vSwapchain::Initialize()
    {
        this->CreateSwapchain();
        this->CreateImages();
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
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
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
}