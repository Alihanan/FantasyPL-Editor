#pragma once

#include "../general/IUncopiable.h"
#include "vDevice.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <map>
#include <vector>


namespace PL
{
    class vShader : public IUncopiable
    {
    public:
        inline static const std::string SHADER_SPIRV_FORMAT = ".spv";
        inline static const std::string SHADER_DEFAULT_PATH = "shaders/";
        static vShader* createShader(std::string name, vDevice* device);

        typedef struct _struct_shader_stages
        {
            const char* name;
            const char* format;
            VkShaderStageFlagBits bit;
        } ShaderStage;
        // Others
        vShader(std::vector<std::string> files, vDevice* device);
    public:
        ~vShader();
        virtual const std::vector<VkPipelineShaderStageCreateInfo>&
            getShaderStages()
        {
            return *this->stages;
        }

        virtual void InitializeLayout()
        {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // Optional
            pipelineLayoutInfo.pSetLayouts = nullptr; // Optional

            auto& pushConstRanges = this->getPushConstantRanges();

            pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstRanges.size()); // Optional
            pipelineLayoutInfo.pPushConstantRanges = pushConstRanges.data(); // Optional
            
            if (vkCreatePipelineLayout(this->device->GetReadyDevice()->logicalDevice,
                    &pipelineLayoutInfo, nullptr, &this->layout) 
                    != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
        }

        std::string GetShaderName() { return this->name; }
        VkPipelineLayout& GetPipelineLayout() { return this->layout; }
        
        virtual const std::vector<VkVertexInputBindingDescription>& 
            getBindingDescriptions() = 0;  
        virtual const std::vector<VkVertexInputAttributeDescription>& 
            getAttributeDescriptions() = 0;
        virtual const std::vector<VkPushConstantRange>&
            getPushConstantRanges() = 0;

        virtual void pushUBOS(VkCommandBuffer buffer) = 0;

    protected:
        vDevice* device = nullptr;

    private:
        inline static std::map<std::string, vShader*> ALL_SHADERS_USED = std::map<std::string, vShader*>();
        typedef struct
        {
            const char* format;
            VkShaderStageFlagBits bit;
        } ShaderType;

        const std::vector<ShaderType> SHADER_TYPES = {
            {".vert", VK_SHADER_STAGE_VERTEX_BIT}, 
            {".frag", VK_SHADER_STAGE_FRAGMENT_BIT}, 
            {".geom", VK_SHADER_STAGE_GEOMETRY_BIT},
            {".tesc",  VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
            {".tese",  VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        };

    protected:
        const std::string name;
        std::vector<VkPipelineShaderStageCreateInfo>* stages;
        const std::vector<std::string> files;
        VkPipelineLayout layout;

        std::vector<VkPipelineShaderStageCreateInfo>* readStages(
            std::vector<std::string> files, VkDevice& device);
        
        std::vector<char>* readFile(const std::string& filename);

        char* extractName(const char* fullname);
   
    };


}