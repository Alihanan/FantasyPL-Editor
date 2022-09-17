#pragma once

#include "../va_uncopiable/va_uncopiable.h"
#include "../v_instance/v_instance.h"
#include "../v_window/v_window.h"

#include <optional>
#include <map>

namespace PL
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    typedef struct _struct_Buffer
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        uint32_t buf_size;

    } Buffer;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    

    class v_device : va_uncopiable
    {
    public:
        static void createDevice(std::string uid);
        static VkDevice& LDevice(std::string uid);
        static SwapChainSupportDetails querySwapchainSupport(std::string uid);
        static QueueFamilyIndices getAvailableQueueFamily(std::string uid);
        static void cleanUp();

    protected:
        static std::map<std::string, v_device*> device_factory;   
        
    protected:       
        v_device();
        ~v_device();

        std::vector<VkPhysicalDevice>& getAvailableDevices();
        void printAvailableDevices();
        bool isDeviceSuitable(VkPhysicalDevice& device);
        QueueFamilyIndices getAvailableQueueFamily(VkPhysicalDevice& device);
        
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device);

        VkQueue& getGraphicsQueue() {return this->graphicsQueue;}
        VkQueue& getPresentQueue() {return this->presentQueue;}

        VkPhysicalDevice& getPhysicalDevice() { return this->physicalDevice; }
        VkDevice& getLogicalDevice() { return this->logicalDevice; }

        void allocateBuffer(
            VkDeviceSize size, 
            VkBufferUsageFlags flags, 
            VkMemoryPropertyFlags properties,
            Buffer* buf);

        void freeBuffer(Buffer* buf);

        void fillBuffer(Buffer* buf, const void* input_data);

        v_instance* instance;
        v_window* window;

        // Physical device
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        void initializeDevice();
        

        // logical device extensions
        std::vector<const char*>* logicalDeviceLayers 
                = new std::vector<const char*>();
        bool checkDeviceExtensionSupport(VkPhysicalDevice& device);

        // Logical device
        VkDevice logicalDevice;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        
        void initializeLogicalDevice();


        // Buffer logic
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
       
    };

}