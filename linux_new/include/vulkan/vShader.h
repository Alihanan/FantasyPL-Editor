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
        const std::vector<VkPipelineShaderStageCreateInfo>&
            getShaderStages()
        {
            return this->stages;
        }
        std::string getShaderName() { return this->name; }

        
        virtual const std::vector<VkVertexInputBindingDescription>& 
            getBindingDescriptions() = 0;  
        virtual const std::vector<VkVertexInputAttributeDescription>& 
            getAttributeDescriptions() = 0;

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
            {".tcs",  VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
            {".tes",  VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        };

    protected:
        const std::string name;
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        const std::vector<std::string> files;

        std::vector<VkPipelineShaderStageCreateInfo> readStages(
            std::vector<std::string> files, VkDevice& device);
        
        std::vector<char>* readFile(const std::string& filename);

        char* extractName(const char* fullname);
   
    };


}