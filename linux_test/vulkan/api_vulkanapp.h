#pragma once
#include "../lib_include.h"
#include "va_uncopiable/va_uncopiable.h"

namespace PL
{
    class API_vulkanapp : va_uncopiable
    {
    public:
        API_vulkanapp() {}
        ~API_vulkanapp() {}

        virtual void initializeVulkan(std::string name) = 0;
        virtual void createWindow(uint32_t W, uint32_t H) = 0;
        virtual void createModels(std::vector<const char*> files) = 0;
        virtual void mainLoop() = 0;        
        
    };

}