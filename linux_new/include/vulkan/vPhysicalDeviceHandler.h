#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vWindow.h"
#include "vInstance.h"
#include <optional>

namespace PL
{
    class vPhysicalDeviceHandler : public IUncopiable, public IDependent
    {
    public:        
        enum Priority 
        {
            DOESNT_MATTER = 0, PREFERRED_IF_POSSIBLE = 1, REQUIRED = 2 
        };
        struct ReadyToUseDevice
        {
            VkPhysicalDevice physicalDevice;
            VkDevice logicalDevice;

            VkQueue graphicsQueue;
            VkQueue presentQueue;
            
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;

            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            VkPhysicalDeviceMemoryProperties memoryProperties;
        };

        // IDependent
        inline const static std::string _DEP_ID = "vPhysicalDeviceHandler";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vWindow::_DEP_ID, vInstance::_DEP_ID
        };

        vPhysicalDeviceHandler()
        {

        }
        
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {
            this->window = static_cast<vWindow*>(context[0][0]);
            this->instance = static_cast<vInstance*>(context[1][0]);
            this->Initialize();
        }
        
        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {          
        }

        bool IsSingleton()
        {
            return true;
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        // Other members
        ~vPhysicalDeviceHandler();
        void SetHighPrerequisities() {this->currentPrereq = prerequisiteHighSetup; }
        void SetLowPrerequiisities() {this->currentPrereq = prerequisiteLowSetup; }

        ReadyToUseDevice* GenerateLogicalDevice(); 

    protected:
        vWindow* window;
        vInstance* instance;
        
        struct Handler
        {
            VkPhysicalDevice physicalDevice;

            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;

            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;

            std::vector<VkExtensionProperties> availableExtensions;

            VkPhysicalDeviceMemoryProperties memoryProperties;

            bool isCompleteQueueuFamily() {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        Handler* currentPhysicalDeviceHandler;
        void Initialize();
        void PickPhysicalDevice();

        Handler* createHandler(VkPhysicalDevice& device);
        void setQueueFamilies(VkPhysicalDevice& device, Handler& handler);
        void setProperties(VkPhysicalDevice& device, Handler& handler);
        void setKHRsettings(VkPhysicalDevice& device, Handler& handler);
        void setSupportedExtensions(VkPhysicalDevice& device, Handler& handler);

        std::vector<VkPhysicalDevice>& listAllPhysicalDevices();
        bool CheckCandidate(Handler* candidate);
        bool CheckObligatory(Handler* candidate);
        bool CheckPrerequisities(Handler* candidate);

        struct Prerequisities
        {
            Priority shouldBeGPU;
            Priority shouldBeDiscrete;
            Priority shouldSupportGeometryShader;
            Priority shouldSupportTessellationShader;
        };

        inline static const Prerequisities prerequisiteHighSetup = 
        { 
            REQUIRED,// shouldBeGPU;
            PREFERRED_IF_POSSIBLE,// shouldBeDiscrete;
            REQUIRED,// shouldSupportGeometryShader;
            REQUIRED// shouldSupportTessellationShader;
        }; 
        inline static const Prerequisities prerequisiteLowSetup = 
        { 
            DOESNT_MATTER,// shouldBeGPU;
            DOESNT_MATTER,// shouldBeDiscrete;
            REQUIRED,// shouldSupportGeometryShader;
            REQUIRED// shouldSupportTessellationShader;
        }; 

        Prerequisities currentPrereq = prerequisiteHighSetup;
        inline static const std::string LAYER_KHR_SWAPCHAIN = "VK_KHR_swapchain";
        inline static const std::vector<const char*> obligatoryLayers = 
        {
            LAYER_KHR_SWAPCHAIN.c_str()
        };
    };
}