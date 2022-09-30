#include "../../include/vulkan/vPhysicalDeviceHandler.h"
#include "../../include/vulkan/vApplication.h"

#include <cstring>
#include <set>

namespace PL
{
    const std::string vPhysicalDeviceHandler::_DEP_ID = IDependent::type(vPhysicalDeviceHandler());

    vPhysicalDeviceHandler::~vPhysicalDeviceHandler()
    {

    }
    
    void vPhysicalDeviceHandler::Initialize()
    {

    }

    vPhysicalDeviceHandler::ReadyToUseDevice* vPhysicalDeviceHandler::GenerateLogicalDevice()
    {
        this->PickPhysicalDevice();
        if(this->currentPhysicalDeviceHandler == VK_NULL_HANDLE)
        {
            return nullptr;
        }
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            currentPhysicalDeviceHandler->graphicsFamily.value(), 
            currentPhysicalDeviceHandler->presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // ask for features
        VkPhysicalDeviceFeatures deviceFeatures{};
        //deviceFeatures.tessellationShader = VK_TRUE;
        //deviceFeatures.geometryShader = VK_TRUE;
        
        // add everything for struct
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.ppEnabledLayerNames = obligatoryLayersVal.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(obligatoryLayersVal.size());

        createInfo.enabledExtensionCount = static_cast<uint32_t>(obligatoryExtensions.size());
        createInfo.ppEnabledExtensionNames = obligatoryExtensions.data();

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        ReadyToUseDevice* ret = new ReadyToUseDevice;
        // Create by struct
        if (vkCreateDevice(currentPhysicalDeviceHandler->physicalDevice, &createInfo, nullptr, 
                    &ret->logicalDevice) 
                    != VK_SUCCESS) {
            delete ret;
            return nullptr;
        }
        // Set Queue
        vkGetDeviceQueue(ret->logicalDevice, 
                        currentPhysicalDeviceHandler->graphicsFamily.value(), 
                        0, &ret->graphicsQueue);
        vkGetDeviceQueue(ret->logicalDevice, 
                        currentPhysicalDeviceHandler->presentFamily.value(), 
                        0, &ret->presentQueue);        

        ret->physicalDevice = currentPhysicalDeviceHandler->physicalDevice;

        ret->formats = currentPhysicalDeviceHandler->formats;
        ret->capabilities = currentPhysicalDeviceHandler->capabilities;
        ret->presentModes = currentPhysicalDeviceHandler->presentModes;

        ret->graphicsFamily = currentPhysicalDeviceHandler->graphicsFamily;
        ret->presentFamily = currentPhysicalDeviceHandler->presentFamily;

        ret->memoryProperties = currentPhysicalDeviceHandler->memoryProperties;

        return ret;
    }

    void vPhysicalDeviceHandler::PickPhysicalDevice()
    {
        this->currentPhysicalDeviceHandler = VK_NULL_HANDLE;
        auto& allDevices = this->listAllPhysicalDevices();
        for(auto device : allDevices)
        {
            Handler* handler = this->createHandler(device);
            bool isSuitable = this->CheckCandidate(handler);
            if(isSuitable)
            {
                this->currentPhysicalDeviceHandler = handler;
                break;
            }
            delete handler;
        }
        allDevices.clear();
        allDevices.shrink_to_fit();
        delete &allDevices;
    }


    
    std::vector<VkPhysicalDevice>& vPhysicalDeviceHandler::listAllPhysicalDevices()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(this->instance->GetInstance(),
                &deviceCount, nullptr);

        if(deviceCount == 0)
        {
            vApplication::GLOBAL_LOGGER << LOG_MSG_ERROR << "No GPU found!" << std::endl;
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice>* devices =
            new std::vector<VkPhysicalDevice>(deviceCount);
        vkEnumeratePhysicalDevices(this->instance->GetInstance(), 
                &deviceCount, devices->data());
        
        return *devices;
    }

    vPhysicalDeviceHandler::Handler* vPhysicalDeviceHandler::createHandler(VkPhysicalDevice& device)
    {
        Handler* ret = new Handler;
        ret->physicalDevice = device;

        this->setQueueFamilies(device, *ret);
        this->setProperties(device, *ret);
        this->setKHRsettings(device, *ret);
        this->setSupportedExtensions(device, *ret);

        return ret;
    }

    void vPhysicalDeviceHandler::setQueueFamilies(VkPhysicalDevice& device, Handler& handler)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());        

        // Used optional so that <no value> can be set        
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                handler.graphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, 
                            this->window->GetKHRSurface(), 
                            &presentSupport);
            if(res != VK_SUCCESS)
            {
                throw std::runtime_error("error getting device surface support!");
            }

            if (presentSupport) {
                handler.presentFamily = i;
            }
            if(handler.isCompleteQueueuFamily())
            {
                break;
            }
            i++;
        }
    }
    void vPhysicalDeviceHandler::setProperties(VkPhysicalDevice& device, Handler& handler)
    {
        vkGetPhysicalDeviceProperties(device, &handler.deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &handler.deviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(device, &handler.memoryProperties);
    }
    void vPhysicalDeviceHandler::setKHRsettings(VkPhysicalDevice& device, Handler& handler)
    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->window->GetKHRSurface(), &handler.capabilities);        

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->window->GetKHRSurface(), &formatCount, nullptr);

        if (formatCount != 0) 
        {
            handler.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->window->GetKHRSurface(), &formatCount, handler.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->window->GetKHRSurface(), &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            handler.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->window->GetKHRSurface(), &presentModeCount, 
                    handler.presentModes.data());
        }
    }

    void vPhysicalDeviceHandler::setSupportedExtensions(VkPhysicalDevice& device, Handler& handler)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        //handler.availableExtensions.resize(extensionCount);
        handler.availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, handler.availableExtensions.data());
    }

    bool vPhysicalDeviceHandler::CheckCandidate(Handler* candidate)
    {
        bool oblig = this->CheckObligatory(candidate);
        if(!oblig) return false;

        return this->CheckPrerequisities(candidate);
    }
    bool vPhysicalDeviceHandler::CheckObligatory(Handler* candidate)
    {
        bool firstCond = candidate->isCompleteQueueuFamily();
        
        bool nonEmptySWAPformats = !candidate->formats.empty();
        bool nonEmptySWAPpresentmodes = !candidate->presentModes.empty();
        bool secondCond = nonEmptySWAPformats && nonEmptySWAPpresentmodes;

        
        std::set<const char*> requiredExtensions(
            obligatoryExtensions.begin(), 
            obligatoryExtensions.end());

        for (const auto& extension : candidate->availableExtensions) {   
            for(const auto& req : requiredExtensions) 
            {
                std::string str1(req);
                std::string str2(extension.extensionName);

                if(str1 == str2)
                {
                    std::string res_str = std::string(req);
                    requiredExtensions.erase(req);

                    vApplication::GLOBAL_LOGGER << LOG_MSG_INFO << 
                                "Ext: " << res_str << " is available!\n";
                    vApplication::GLOBAL_LOGGER << 
                                "(" << str2 << ")" << std::endl;
                    break;
                }
            }
        }        
        bool thirdCond = requiredExtensions.empty();


        return firstCond && secondCond && thirdCond;
    }
    bool vPhysicalDeviceHandler::CheckPrerequisities(Handler* candidate)
    {
        bool isDiscreteGPU = (candidate->deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        bool isIntegratedGPU = (candidate->deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
        bool isGPU = isDiscreteGPU || isIntegratedGPU;
        
        if((this->currentPrereq.shouldBeGPU == REQUIRED) && !isGPU) return false;

        if(isGPU && (this->currentPrereq.shouldBeDiscrete == REQUIRED) && !isDiscreteGPU) return false;

        bool isGS = candidate->deviceFeatures.geometryShader;
        bool isTS = candidate->deviceFeatures.tessellationShader;

        if((this->currentPrereq.shouldSupportGeometryShader == REQUIRED) && !isGS) return false;

        if((this->currentPrereq.shouldSupportTessellationShader == REQUIRED) && !isTS) return false;

        return true;
    }   

}