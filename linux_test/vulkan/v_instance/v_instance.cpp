#include "v_instance.h"
#include "../../math/algorithms.h"

using std::to_string;

namespace PL
{
    v_instance* v_instance::singleton = nullptr;

    void v_instance::createV_instance(std::string app_name)
    {
        if(singleton == nullptr) 
            singleton = new v_instance(app_name);
    }
    VkInstance& v_instance::getV_instance()
    {
        if(singleton == nullptr) 
            throw new std::runtime_error("Trying to get instance before creation!");
        return singleton->getInstance();
    }
    std::string v_instance::getAppName()
    {
        if(singleton == nullptr) 
            throw new std::runtime_error("Trying to get instance before creation!");
        return singleton->getName();
    }
    void v_instance::cleanUp()
    {
        if(singleton != nullptr)
            delete singleton;
    }

    /**
     *  END OF STATIC PART
     */
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);


    v_instance::v_instance(std::string app_name)
    {
        this->appName = app_name;
        
        this->setDefaultInstance();  
        
        this->createInstance();  
        if(this->enableValidationLayers)
        {
            this->setupDebugger();
            if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }        
    }
    v_instance::~v_instance()
    {
        if(this->enableValidationLayers)
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroyInstance(instance, nullptr);
        delete this->extensions;
        delete this->layers;
    }
    
    void v_instance::createInstance()
    {
        VkResult result = vkCreateInstance(&(this->createInfo), nullptr, &this->instance);
        if(result != VK_SUCCESS){
            throw std::runtime_error("VkInstance creation error!");
        }
    }

    void v_instance::setDefaultInstance()
    {
        this->setAppInfo();
        this->setCreateInfo();
    }

    void v_instance::setCreateInfo()
    {
        this->createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        this->createInfo.pApplicationInfo = &(this->appInfo);

        this->setEnabledExtensions();      
        this->setEnabledLayers();
    }

    void v_instance::setAppInfo()
    {
        this->appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        this->appInfo.pApplicationName = this->appName.c_str();
        this->appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        this->appInfo.pEngineName = "No Engine";
        this->appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        this->appInfo.apiVersion = VK_API_VERSION_1_0;

    }
    

    std::vector<VkLayerProperties>* v_instance::getAvailableLayers()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties>* availableLayers = 
            new std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers->data());

        return availableLayers;
    }
    void v_instance::setEnabledLayers()
    {
        this->layers = new std::vector<const char*>();
        // needed validation layer       
        if(this->enableValidationLayers){
            layers->push_back(LAYER_KHR_VALIDATION.c_str());
            //this->createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &createInfo;           
        }

        createInfo.enabledLayerCount = static_cast<uint32_t>(layers->size());
        createInfo.ppEnabledLayerNames = layers->data();

        this->assertLayers();
    }   
    void v_instance::assertLayers()
    {
        // available
        std::vector<VkLayerProperties>* exts = this->getAvailableLayers();
        std::vector<std::string> available;
        available.reserve(exts->size());
        for(const auto& ext : *exts)
        {
            std::string n_ext(ext.layerName);
            available.push_back(n_ext);
        }
        // needed
        std::vector<std::string> needed;
        needed.reserve(this->createInfo.enabledLayerCount);       
        
        for(uint32_t i=0; i < this->createInfo.enabledLayerCount; i++)
        {
            std::string name_ext(this->createInfo.ppEnabledLayerNames[i]);
            needed.push_back(name_ext);
        }

        // check availability
        std::vector<std::string> diff = PL::differenceSubsets(needed, available);
        delete exts;

        if(diff.size() == 0){
            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << 
            "[Layers] All needed layers are supported! " <<
            this->createInfo.enabledLayerCount <<
            " enabled in total!" << std::endl;

            return;
        }     
        
        Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << 
        "Needed layers not supported on your system! (" << diff.size() <<
        " of " << this->createInfo.enabledLayerCount << " not supported!)\n";
        
        for(const auto& elem : diff){
            Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << "\t" << elem << std::endl;
        }
    }

    void v_instance::setEnabledExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        // + 178,082 bytes allocated
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        
        this->extensions = new 
        std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
        Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "GLFW extensions needed:" << std::endl;
        for(const char* name : *this->extensions)
        {
            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "\t" << name << std::endl;
        }

        extensions->push_back(LAYER_EXT_DEBUG_UTILS.c_str());

        this->createInfo.enabledExtensionCount = 
                    static_cast<uint32_t>(this->extensions->size());
        this->createInfo.ppEnabledExtensionNames = extensions->data();

        this->assertExtensions();   
    }

    std::vector<VkExtensionProperties>* v_instance::getAvailableExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties>* extensions = new 
                            std::vector<VkExtensionProperties>(extensionCount);
        
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 
                                                extensions->data());

        return extensions;
    }


    void v_instance::printAvailableExtensions()
    {
        std::vector<VkExtensionProperties>* exts = this->getAvailableExtensions();
        Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "available extensions:\n";

        for (const auto& extension : *exts) {
            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << '\t' << extension.extensionName << '\n';
        }

        delete &exts;
    }
    
    void v_instance::assertExtensions()
    {
        // available
        std::vector<VkExtensionProperties>* exts = this->getAvailableExtensions();    

        std::vector<std::string> available;
        available.reserve(exts->size());
        for(const auto& ext : *exts)
        {
            std::string n_ext(ext.extensionName);
            available.push_back(n_ext);
        }
        // needed
        std::vector<std::string> needed;
        needed.reserve(this->createInfo.enabledExtensionCount);       
        
        for(uint32_t i=0; i < this->createInfo.enabledExtensionCount; i++)
        {
            std::string name_ext(this->createInfo.ppEnabledExtensionNames[i]);
            needed.push_back(name_ext);
        }

        // check availability
        std::vector<std::string> diff = PL::differenceSubsets(needed, available);
        delete exts;
        needed.clear();
        available.clear();

        if(diff.size() == 0){
            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "[Extensions] All needed extensions are supported! " 
            << this->createInfo.enabledExtensionCount << " enabled in total!" << std::endl;
            return;
        }     

        Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << "Needed extensions not supported on your system! (" << diff.size() 
        << " of " << this->createInfo.enabledExtensionCount <<
           " not supported!)" << std::endl;
        for(const auto& elem : diff){
            Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << '\t' << elem << std::endl;
        }    
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }


    
    void v_instance::setupDebugger()
    {   
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = nullptr; // Optional
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    


}