#include "../../include/vulkan/vDevice.h"
#include "../../include/vulkan/vApplication.h"

namespace PL
{
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

}