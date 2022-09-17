#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 

namespace PL
{
    class vInstance : public IUncopiable, public IDependent
    {
    public:
        // IDependent
        inline const static std::string _DEP_ID = "vInstance";
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {};

        vInstance()
        : appName(DEFAULT_APP_NAME)
        {}
        
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }

        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {this->Initialize();}
        
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
        ~vInstance();
        VkInstance& GetInstance() { return this->instance; }
        std::string GetName() { return this->appName; }

    protected:
        void Initialize();
        VkInstance instance {};
        const std::string appName;
        
        // Variable constants
        inline const static std::string DEFAULT_APP_NAME = "Fantasy PL Editor";
        // Layer name constants
        inline static const std::string LAYER_KHR_VALIDATION = "VK_LAYER_KHRONOS_validation";
        inline static const std::string LAYER_KHR_SWAPCHAIN = "VK_KHR_swapchain";
        inline static const std::string LAYER_EXT_DEBUG_UTILS = "VK_EXT_debug_utils";
        #ifndef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        

    private:
        void InitializeInstance();
        void DestroyInstance();

        void InitializeLayersAndExtensions();
        void DestroyLayersAndExtensions();

        // layers
        std::vector<const char*>* layers;
        void InitLayers();
        void CheckAvailableLayers();
        void DestroyLayers() { 
            delete layers; 
        }
        // extensions
        std::vector<const char*>* extensions;
        void InitExtensions();
        void CheckAvailableExtensions();
        void DestroyExtensions() { 
            delete this->extensions; 
        }
        // debugger
        VkDebugUtilsMessengerEXT debugMessenger;
        void InitializeDebugger();
        void DestroyDebugger();
    };
}