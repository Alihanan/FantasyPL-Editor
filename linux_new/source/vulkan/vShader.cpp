#include "../../include/vulkan/vShader.h"

#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

namespace PL
{
    const std::string vShader::_DEP_ID = IDependent::type(vShader({}));

     std::map<std::string, vShader*> vShader::ALL_SHADERS_USED = 
                    std::map<std::string, vShader*>();

    vShader::vShader(std::vector<std::string> files) 
    :   name(this->extractName((
            files.size() == 0) ? "" :  files[0].c_str()
            )),
        files(files)
    {}

    vShader::~vShader()
    {
        
    }
    
    void vShader::Initialize()
    {
        this->readStages(this->files, this->device->GetReadyDevice()->logicalDevice);
    }

    char* vShader::extractName(const char* fullname)
    {
        if(strcmp(fullname, "") == 0)
        {
            char* ret = new char[1];
            ret[0] = 0;
            return ret;
        }

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


    std::vector<VkPipelineShaderStageCreateInfo> vShader::readStages(
            std::vector<std::string> files, VkDevice& device
            )
    {
        std::vector<VkPipelineShaderStageCreateInfo> vec;// = 
            // std::vector<VkPipelineShaderStageCreateInfo>();1
        for(std::string f : files)
        {
            std::vector<char>* ret = this->readFile(f + vShader::SHADER_SPIRV_FORMAT);
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
            for(uint32_t i = 0; i < SHADER_TYPES.size(); i++)
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

    std::vector<char>* vShader::readFile(const std::string& filename)
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