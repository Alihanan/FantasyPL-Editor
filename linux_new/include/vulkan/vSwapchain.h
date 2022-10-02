#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vDevice.h"
#include "vWindow.h"
#include "vRenderPass.h"

namespace PL
{
    class vSwapchain : public IUncopiable, public IDependent
    {
    public:
        inline const static uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        // IDependent
        const static std::string _DEP_ID;
        // inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
        //     vDevice::_DEP_ID, vWindow::_DEP_ID, vRenderPass::_DEP_ID
        // };
        std::vector<std::string> GetNeededDependencies()
        {
            return {vDevice::_DEP_ID, vWindow::_DEP_ID, vRenderPass::_DEP_ID};
        }
        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {
            this->device = static_cast<vDevice*>(context[0][0]);
            this->window = static_cast<vWindow*>(context[1][0]);
            this->renderPass = static_cast<vRenderPass*>(context[2][0]);
            this->Initialize();
        }
        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
        }

        // Other members
        ~vSwapchain();
        size_t GetImageCount() {return this->swapChainImages.size();}
        VkRenderPass& GetRenderPass() { return this->renderPass->GetRenderPass(); }
        VkFramebuffer& GetActiveFramebuffer(uint32_t index) { return this->swapChainFramebuffers[index];}
        VkExtent2D GetExtent() { return this->swap_extent; }

        bool AcquireImage(uint32_t* imageIndex);
        bool SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        VkSwapchainKHR& GetSwapchain() {return this->swapChain;}
        
        //void RecreateOnResize(vWindow* activeWindow);
        void Recreate();
        bool CheckWindowResize();

    protected:
        vDevice* device;
        vWindow* window;
        vRenderPass* renderPass;

        void Initialize();
    private:
        uint32_t currentFrame = 0;

        std::vector<VkImage> swapChainImages = std::vector<VkImage>();
        std::vector<VkImageView> swapChainImageViews = std::vector<VkImageView>();
        void CreateImages();

        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;
        void CreateSwapchainKHR();
        void RecreateSwapchain();
        void DeleteSwapchain();

        std::vector<VkFramebuffer> swapChainFramebuffers;
        void CreateFramebuffers();
        
        VkPresentModeKHR swap_presentMode {};
        VkExtent2D swap_extent {};
        
        
        VkPresentModeKHR ChooseSWAPPresentMode();
        VkExtent2D ChooseSWAPExtent();
        uint32_t ChooseImageCount();
        void InitializeSwapchain();


        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        void InitializeSynchronization();
        void DeleteSynchronization();        
    };
}