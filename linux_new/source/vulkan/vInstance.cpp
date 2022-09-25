#include "../../include/vulkan/vInstance.h"
#include "../../include/math/algorithms.h"
#include "../../include/vulkan/vApplication.h"

namespace PL
{   
    const std::string vInstance::_DEP_ID = IDependent::type(vInstance());

    static void glfwErrorCallback(int code, const char* description);


    void vInstance::Initialize()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwSetErrorCallback(glfwErrorCallback);

        this->InitializeLayersAndExtensions();   
        this->InitializeInstance();
        this->InitializeDebugger();
    }

    vInstance::~vInstance()
    {
        this->DestroyDebugger();
        this->DestroyInstance();
        this->DestroyLayersAndExtensions();

        glfwTerminate();
    }


    void vInstance::InitializeInstance()
    {
        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = this->appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &(appInfo);

        // extensions
        createInfo.enabledExtensionCount = 
                    static_cast<uint32_t>(this->extensions->size());
        createInfo.ppEnabledExtensionNames = extensions->data();
        // layers
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers->size());
        createInfo.ppEnabledLayerNames = layers->data();

        VkResult result = vkCreateInstance(&(createInfo), nullptr, &this->instance);
        if(result != VK_SUCCESS){
            throw std::runtime_error("VkInstance creation error!");
        }
    }
    void vInstance::DestroyInstance()
    {
        vkDestroyInstance(instance, nullptr);
    }


    void vInstance::InitializeLayersAndExtensions()
    {
        this->InitExtensions();
        this->InitLayers();
    }

    
    void vInstance::InitLayers()
    {
        this->layers = new std::vector<const char*>({});

        if(this->enableValidationLayers)
            this->layers->push_back(LAYER_KHR_VALIDATION.c_str());

        this->CheckAvailableLayers();
    }
    void vInstance::CheckAvailableLayers()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties>* exts = 
            new std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, exts->data());

        // available
        std::vector<std::string> available;
        available.reserve(exts->size());
        for(const auto& ext : *exts)
        {
            std::string n_ext(ext.layerName);
            available.push_back(n_ext);
        }
        // needed
        std::vector<std::string> needed;
        needed.reserve(this->layers->size());       
        
        for(uint32_t i=0; i < this->layers->size(); i++)
        {
            std::string name_ext((*this->layers)[i]);
            needed.push_back(name_ext);
        }

        // check availability
        std::vector<std::string> diff = PL::differenceSubsets(needed, available);
        delete exts;

        if(diff.size() != 0)
        {
           vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << "Needed layers not supported on your system! (" << diff.size() 
            << " of " << this->layers->size() <<
            " not supported!)" << std::endl;
            for(const auto& elem : diff){
                vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << '\t' << elem << std::endl;
            }  

            throw std::runtime_error("Layers check failed!");  
        }
    }
    void vInstance::InitExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        // + 178,082 bytes allocated
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        this->extensions = new 
            std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);


        if(this->enableValidationLayers)
            this->extensions->push_back(LAYER_EXT_DEBUG_UTILS.c_str());
        
        this->CheckAvailableExtensions();
    }
    void vInstance::CheckAvailableExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties>* avalExt = new 
                            std::vector<VkExtensionProperties>(extensionCount);
        
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 
                                                avalExt->data());
        
        // available
        std::vector<std::string> available;
        available.reserve(avalExt->size());
        for(const auto& ext : *avalExt)
        {
            std::string n_ext(ext.extensionName);
            available.push_back(n_ext);
        }
        // needed
        std::vector<std::string> needed;
        needed.reserve(this->extensions->size());       
        
        for(uint32_t i=0; i < this->extensions->size(); i++)
        {
            std::string name_ext((*this->extensions)[i]);
            needed.push_back(name_ext);
        }

        // check availability
        std::vector<std::string> diff = PL::differenceSubsets(needed, available);
        delete avalExt;

        if(diff.size() != 0)
        {
           vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << "Needed extensions not supported on your system! (" << diff.size() 
            << " of " << this->extensions->size() <<
            " not supported!)" << std::endl;
            for(const auto& elem : diff){
                vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << '\t' << elem << std::endl;
            }  

            throw std::runtime_error("Extension check failed!");  
        }
    }   
    void vInstance::DestroyLayersAndExtensions()
    {
        this->DestroyExtensions();
        this->DestroyLayers();
    }

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) 
    {
        if(messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    
    static void glfwErrorCallback(int code, const char* description)
    {
        vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << "GLFW Error " << code << ": " << description << std::endl;
    }

    void vInstance::InitializeDebugger()
    {
        if(!this->enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = nullptr; // Optional

        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    void vInstance::DestroyDebugger()
    {
        if(!this->enableValidationLayers) return;
        if(this->debugMessenger == VK_NULL_HANDLE) return;
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }


}