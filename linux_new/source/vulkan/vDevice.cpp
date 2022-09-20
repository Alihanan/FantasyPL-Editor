#include "../../include/vulkan/vDevice.h"
#include "../../include/vulkan/vApplication.h"

namespace PL
{
    const std::string vDevice::_DEP_ID = IDependent::type(vDevice()); 

    vDevice::~vDevice()
    {

    }

    vPhysicalDeviceHandler::ReadyToUseDevice* vDevice::GetReadyDevice()
    {
        return this->device;
    }

    void vDevice::Initialize()
    {
        this->physicalDeviceHandler->SetHighPrerequisities();
        this->device = this->physicalDeviceHandler->GenerateLogicalDevice();

        if(this->device != nullptr) return;

        this->physicalDeviceHandler->SetLowPrerequiisities();
        this->device = this->physicalDeviceHandler->GenerateLogicalDevice();

        if(this->device == nullptr)
        {    
            throw std::runtime_error("failed to create logical device!");
        }
    }

    uint32_t vDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(this->device->physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

}