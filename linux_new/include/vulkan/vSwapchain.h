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
        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vDevice::_DEP_ID, vWindow::_DEP_ID, vRenderPass::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        bool IsSingleton()
        {
            return true;
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

    protected:
        vDevice* device;
        vWindow* window;
        vRenderPass* renderPass;

        void Initialize();
    private:


        std::vector<VkImage> swapChainImages = std::vector<VkImage>();
        std::vector<VkImageView> swapChainImageViews = std::vector<VkImageView>();
        void CreateImages();

        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;
        void CreateSwapchain();

        
        VkPresentModeKHR swap_presentMode;
        VkExtent2D swap_extent;
        
        
        VkPresentModeKHR ChooseSWAPPresentMode();
        VkExtent2D ChooseSWAPExtent();
        uint32_t ChooseImageCount();
    };
}