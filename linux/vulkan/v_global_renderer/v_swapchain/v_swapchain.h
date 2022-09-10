#pragma once
#include "../../../lib_include.h"
#include "../../va_uncopiable/va_uncopiable.h"
#include "../../v_device/v_device.h"
#include "v_renderpass/v_renderpass.h"

namespace PL
{
    class v_swapchain : va_uncopiable
    {
    public:
        static v_swapchain* currentSwapchain;
        
        static void createV_swapchain();
        static void recreateSwapchain();
        static VkResult acquireNextImage(uint32_t* imageIndex, VkSemaphore& semaphor);
        static VkResult presentResult(std::vector<VkCommandBuffer>& bufs, VkSemaphore& semaphore);
        static void cleanUp();

    protected:
        v_swapchain();
        ~v_swapchain();
        VkSwapchainKHR& getSwapchain() { return this->swapChain;}
        VkExtent2D& getSwapchainExtent() { return this->swapChainExtent;}
        VkFormat& getSwapchainImageFormat() { return this->swapChainImageFormat;}
        std::vector<VkFramebuffer>& getFramebuffers() { return this->swapChainFramebuffers;}
        VkRenderPass& getRenderPass() {return this->renderpass->getRenderPass();}
        size_t getImageCount() {return this->swapChainImages.size();}

        //std::vector<VkImageView>& getSwapChainImageViews() 
        //{ return this->swapChainImageViews; }

        VkResult acquireImg(uint32_t* imageIndex, VkSemaphore& semaphor);
        VkResult present(std::vector<VkCommandBuffer>& bufs, VkSemaphore& semaphore);
        void recreate(VkExtent2D newExtent);
    private:
        void init();
        void deleteSwapchain();
    protected:
        //v_device* device;
        //v_window* window;

        // dynamic variables
        v_renderpass* renderpass = VK_NULL_HANDLE;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;
        std::vector<VkImage> swapChainImages = std::vector<VkImage>();
        std::vector<VkImageView> swapChainImageViews = std::vector<VkImageView>();

        // fixed variables
        VkSurfaceFormatKHR surfaceFormat;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        VkExtent2D windowPixelExtent;

        void initializeSwapchain();
        void initiailizeImageViews();

        void createRenderpass();
        void createFramebuffers();
        std::vector<VkFramebuffer> swapChainFramebuffers;
        
        // check device formats/supports
        
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                    const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(
                    const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(
                    const VkSurfaceCapabilitiesKHR& capabilities);
    };




}