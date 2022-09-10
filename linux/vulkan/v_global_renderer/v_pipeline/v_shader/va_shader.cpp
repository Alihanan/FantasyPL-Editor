#include "va_shader.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace PL
{
    std::map<std::string, va_shader*> va_shader::ALL_SHADERS_USED = 
                    std::map<std::string, va_shader*>();

    va_shader::~va_shader()
    {
        delete &(this->stages);
    }

    char* va_shader::extractName(const char* fullname)
    {
        size_t size = strlen(fullname);

        // skip *.frag or similar
        while(size > 0)
        {
            char c = fullname[size-1];
            if(c == '.')
            {
                break;
            }
            size--;
        }

        size_t end = size - 1;
        
        while(size > 0)
        {
            char c = fullname[size - 1];
            if(c == '/')
            {
                break;
            }
            size--;
        }
        size_t start = size;

        char* ret = new char[end - start + 1];
        for(size_t i = start; i < end; i++)
        {
            ret[i - start] = fullname[i];
        }
        ret[end - start] = '\0';
        return ret;
    }


    std::vector<VkPipelineShaderStageCreateInfo> va_shader::readStages(
            std::vector<std::string> files, VkDevice& device
            )
    {
        std::vector<VkPipelineShaderStageCreateInfo> vec;// = 
            // std::vector<VkPipelineShaderStageCreateInfo>();1
        for(std::string f : files)
        {
            std::vector<char>* ret = this->readFile(f + SHADER_SPIRV_FORMAT);
            VkShaderModuleCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = ret->size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(ret->data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device,
                &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {

                throw std::runtime_error("failed to create shader module!");
            }

            delete ret;


            VkPipelineShaderStageCreateInfo shaderStageInfo;
            
            shaderStageInfo.pName = "main";
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.module = shaderModule;
            shaderStageInfo.flags = 0;
            shaderStageInfo.pNext = nullptr;
            shaderStageInfo.pSpecializationInfo = nullptr;
            
            size_t size = strlen(f.c_str());
            const char* format = (f.c_str() + size - 5);
            for(uint32_t i = 0; i < NUM_SHADER_TYPES; i++)
            {
                if(strcmp(format, SHADER_TYPES[i].format) == 0)
                {
                    shaderStageInfo.stage = SHADER_TYPES[i].bit;
                    break;
                }
            }
            vec.push_back(shaderStageInfo);
        }

        auto sortRuleLambda = [] (VkPipelineShaderStageCreateInfo const& s1, VkPipelineShaderStageCreateInfo const& s2) -> bool
        {
        return s1.stage < s2.stage;
        };
        std::sort(vec.begin(), vec.end(), sortRuleLambda);

        return vec;
    }

    std::vector<char>* va_shader::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        std::vector<char>* buffer = new std::vector<char>();

        if (!file.is_open()) {
            return buffer;
        }
        size_t fileSize = (size_t) file.tellg();
        buffer->resize(fileSize);

        file.seekg(0);
        file.read(buffer->data(), fileSize);
        file.close();

        return buffer;
    }
}