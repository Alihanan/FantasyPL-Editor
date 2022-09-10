#include "v_device.h"
#include "../../lib_include.h"
// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// standard libs
#include <iostream>
#include <cstdlib>
#include <set>
#include <cstring>

namespace PL
{
    std::map<std::string, v_device*> v_device::device_factory =
            std::map<std::string, v_device*>();


    void v_device::createDevice(std::string uid)
    {
        if(v_device::device_factory.find(uid) == v_device::device_factory.end())
        {
            v_device::device_factory[uid] = new v_device();
        }
    }
    VkDevice& v_device::LDevice(std::string uid)
    {
        if(v_device::device_factory.find(uid) == v_device::device_factory.end())
        {
            throw std::runtime_error("trying to access device before creation!");
        }
        return v_device::device_factory[uid]->getLogicalDevice();
    }

    SwapChainSupportDetails v_device::querySwapchainSupport(std::string uid)
    {
        if(v_device::device_factory.find(uid) == v_device::device_factory.end())
        {
            throw std::runtime_error("trying to access device before creation!");
        }
        v_device* dev = v_device::device_factory[uid];
        return dev->querySwapChainSupport(dev->getPhysicalDevice());
    }

    QueueFamilyIndices v_device::getAvailableQueueFamily(std::string uid)
    {
        if(v_device::device_factory.find(uid) == v_device::device_factory.end())
        {
            throw std::runtime_error("trying to access device before creation!");
        }
        v_device* dev = v_device::device_factory[uid];
        return dev->getAvailableQueueFamily(dev->getPhysicalDevice());
    }

    void v_device::cleanUp()
    {
        for(const auto& [key, value] : v_device::device_factory)
        {
            delete value;
        }
    }

    /**
     *  END OF STATIC PART
     */
    v_device::v_device()
    {
        //this->instance = inst;
        //this->window = window;
        logicalDeviceLayers->push_back(LAYER_KHR_SWAPCHAIN.c_str());
        
        //this->printAvailableDevices();
        this->initializeDevice();
        this->initializeLogicalDevice();

    }

    v_device::~v_device()
    {
        vkDestroyDevice(this->logicalDevice, nullptr);
        delete this->logicalDeviceLayers;
    }

    uint32_t v_device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }



    void v_device::allocateBuffer(
            VkDeviceSize size, 
            VkBufferUsageFlags usage, 
            VkMemoryPropertyFlags properties,
            Buffer* buf)
    {
        buf->buf_size = size;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;//VBO
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //simultaneous use from diff. queue families

        if (vkCreateBuffer(this->logicalDevice,
                &bufferInfo, nullptr, &(buf->buffer)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }


        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->logicalDevice, 
                                    buf->buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = 
            findMemoryType(memRequirements.memoryTypeBits, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


        if (vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, 
                &(buf->memory)) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(this->logicalDevice, buf->buffer, buf->memory, 0);
    }

    void v_device::fillBuffer(Buffer* buf, const void* input_data)
    {
        void* data;
        vkMapMemory(this->logicalDevice, 
            buf->memory, 0, buf->buf_size, 0, &data);
        
        memcpy(data, input_data, (size_t) buf->buf_size);

        vkUnmapMemory(this->logicalDevice, buf->memory);
    }
            
    void v_device::freeBuffer(Buffer* buf)
    {
        vkDestroyBuffer(this->logicalDevice, buf->buffer, nullptr);
        vkFreeMemory(this->logicalDevice, buf->memory, nullptr);
    }

    void v_device::initializeDevice()
    {       
        auto& devices = this->getAvailableDevices();
        
        if (devices.size() == 0) {
            Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << "No GPU found!" << std::endl;
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        for(VkPhysicalDevice pd : devices)
        {
            bool isSuitable = this->isDeviceSuitable(pd);
            if(isSuitable)
            {
                this->physicalDevice = pd;
                break;
            }
        }
        devices.clear();
        devices.shrink_to_fit();
        delete &devices;

        if(this->physicalDevice == VK_NULL_HANDLE)
        {
            Logger::GLOBAL_LOGGER << LOG_MSG_ERROR << "No suitable GPU found!" << std::endl;
            throw new std::runtime_error("failed to find a suitable GPU!");
        }
    }

    std::vector<VkPhysicalDevice>& v_device::getAvailableDevices()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(v_instance::getV_instance(), 
                &deviceCount, nullptr);

        std::vector<VkPhysicalDevice>* devices =
            new std::vector<VkPhysicalDevice>(deviceCount);
        vkEnumeratePhysicalDevices(v_instance::getV_instance(), 
                &deviceCount, devices->data());
        
        return *devices;
    }

    void v_device::printAvailableDevices()
    {
        auto devices = this->getAvailableDevices();
        for(uint32_t i = 0; i < devices.size(); i++)
        {
            VkPhysicalDevice pd = devices[i];
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(pd, &props);

            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(pd, &deviceFeatures);

            Logger::GLOBAL_LOGGER << LOG_MSG_INFO << "|-----------------------------------|\n";
            Logger::GLOBAL_LOGGER << "Device " << i << ":\n";
            Logger::GLOBAL_LOGGER << "Name: " << props.deviceName << "\n";
            Logger::GLOBAL_LOGGER << "Type(1 = INTEGRATED_GPU, 2 = Discrete GPU, 4 = CPU): " << props.deviceType 
                    << "\n";
            Logger::GLOBAL_LOGGER << "Is Geometry shader supported? " << deviceFeatures.geometryShader 
                    << "\n";
            Logger::GLOBAL_LOGGER << "Is Queue family supported? " << 
                            this->getAvailableQueueFamily(pd).isComplete() << "\n";
            Logger::GLOBAL_LOGGER << "|-----------------------------------|" << std::endl;
        }
        devices.clear();
        devices.shrink_to_fit();
        delete &devices;
    }

    bool v_device::isDeviceSuitable(VkPhysicalDevice& device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        bool isGPU = (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
        (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);

        bool isGS = deviceFeatures.geometryShader;
        bool isQFamily = this->getAvailableQueueFamily(device).isComplete();
        bool isSwapChainExtensionSupported = this->checkDeviceExtensionSupport(device);

        bool isSwapChain = false;
        if (isSwapChainExtensionSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            isSwapChain = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return isGPU && isGS && isQFamily && isSwapChain;       
    }

    QueueFamilyIndices v_device::getAvailableQueueFamily(VkPhysicalDevice& device)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());        

        // Used optional so that <no value> can be set
        QueueFamilyIndices indices;
        
        

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }


            VkBool32 presentSupport = false;
            VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, 
                            v_window::getSurface(WINDOW_MAIN), 
                            &presentSupport);
            if(res != VK_SUCCESS)
            {
                throw std::runtime_error("error getting device surface support!");
            }

            if (presentSupport) {
                indices.presentFamily = i;
            }
            if(indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    void v_device::initializeLogicalDevice()
    {       
        QueueFamilyIndices indices = this->getAvailableQueueFamily(this->physicalDevice);
        
        // Create set of unique queues
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(), 
            indices.presentFamily.value()};
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

        // add everything for struct
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pEnabledFeatures = &deviceFeatures;

        //createInfo.enabledExtensionCount = 0;
        // Older vulkan needed extensions with device too
        /*
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }   */    
       
        createInfo.ppEnabledLayerNames = logicalDeviceLayers->data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(logicalDeviceLayers->size());
        //createInfo.enabledLayerCount = 0;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(logicalDeviceLayers->size());
        createInfo.ppEnabledExtensionNames = logicalDeviceLayers->data();

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        // Create by struct
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &this->logicalDevice) 
                    != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // Set Queue
        vkGetDeviceQueue(this->logicalDevice, 
                        indices.graphicsFamily.value(), 
                        0, &graphicsQueue);
        vkGetDeviceQueue(this->logicalDevice, 
                        indices.presentFamily.value(), 
                        0, &presentQueue);        
    }



    bool v_device::checkDeviceExtensionSupport(VkPhysicalDevice& device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<const char*> requiredExtensions(
            this->logicalDeviceLayers->begin(), 
            this->logicalDeviceLayers->end());


        for (const auto& extension : availableExtensions) {       
            for(const auto& req : requiredExtensions)
            {
                if(strcmp(req, extension.extensionName) == 0)
                {
                    std::string res_str = std::string(req);
                    requiredExtensions.erase(req);
                    Logger::GLOBAL_LOGGER << LOG_MSG_INFO << 
                    "Ext: " << res_str << " is available!\n";
                    Logger::GLOBAL_LOGGER << 
                    "(" << extension.extensionName << ")" << std::endl;
                    break;
                }
            }

            
        }        

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails v_device::querySwapChainSupport(VkPhysicalDevice& device)
    {
        VkSurfaceKHR& surface = v_window::getSurface(WINDOW_MAIN);

        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);        

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }


        return details;
    }
} 
