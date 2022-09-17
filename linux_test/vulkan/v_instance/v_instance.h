#pragma once
#include "../va_uncopiable/va_uncopiable.h"
#include "../../lib_include.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 

#include <string>
#include <vector>

namespace PL
{
    class v_instance : va_uncopiable
    {
    public:       
        static void createV_instance(std::string app_name);
        static VkInstance& getV_instance();
        static std::string getAppName();
        static void cleanUp();
    protected:
        static v_instance* singleton;    
    
    protected:
        v_instance(std::string app_name);
    #ifndef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
        
    #endif
        ~v_instance();

        VkInstance& getInstance() {return this->instance;}
        std::vector<VkExtensionProperties>* getAvailableExtensions();
        std::vector<VkLayerProperties>* getAvailableLayers();
        std::string& getName() { return this->appName; }
        void printAvailableExtensions();

        std::vector<const char*>* extensions;
        std::vector<const char*>* layers;
        std::string appName;
        VkApplicationInfo appInfo{};
        VkInstanceCreateInfo createInfo{};
        
        VkInstance instance {};
        
        void createInstance();

        void setDefaultInstance();
        void setCreateInfo();
        void setAppInfo();

        void setEnabledExtensions();      
        void assertValidationExtension();
        void assertExtensions();

        void setEnabledLayers();      
        void assertLayers();        

        // debugger
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        void setupDebugger();

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }
    };
  
} 

