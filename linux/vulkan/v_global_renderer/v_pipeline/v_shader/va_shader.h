#pragma once
#include "../va_uncopiable/va_uncopiable.h"
#include "../../lib_include.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


#include <map>
#include <vector>


#define NUM_SHADER_TYPES 3U

namespace PL
{
    typedef struct _struct_shader_stages
    {
        const char* name;
        const char* format;
        VkShaderStageFlagBits bit;
    } ShaderStage;
    static const ShaderStage SHADER_STAGES[] = {
        {"vertex shader",   ".vert", VK_SHADER_STAGE_VERTEX_BIT}, 
        {"geometry shader", ".geom", VK_SHADER_STAGE_GEOMETRY_BIT},
        {"fragment shader", ".frag", VK_SHADER_STAGE_FRAGMENT_BIT}, 
    };
    static const uint32_t SHADER_STAGE_SIZE = sizeof(SHADER_STAGES)/sizeof(SHADER_STAGES[0]);
    /*                   
    float: VK_FORMAT_R32_SFLOAT
    vec2: VK_FORMAT_R32G32_SFLOAT
    vec3: VK_FORMAT_R32G32B32_SFLOAT
    vec4: VK_FORMAT_R32G32B32A32_SFLOAT      
    */
    class va_shader : va_uncopiable
    {
    public:
        struct ShaderInfo
        {
            VkPipelineShaderStageCreateInfo create_info;                
            VkShaderModule shader_module;
        };

        va_shader(std::vector<std::string> files, VkDevice& d) 
            : name(this->extractName(files[0].c_str())), 
              device(d), 
              stages(readStages(files, d))
        {}

        virtual ~va_shader() = 0; // make abstract

        static va_shader* createShader(std::string name, VkDevice& device);

        const std::vector<VkVertexInputBindingDescription> tmp_binding = {};
        virtual const std::vector<VkVertexInputBindingDescription>& 
            getBindingDescriptions()
        {
            return tmp_binding;
        }

        const std::vector<VkVertexInputAttributeDescription> tmp_attrib = {};
        virtual const std::vector<VkVertexInputAttributeDescription>& 
            getAttributeDescriptions()
        {
            return tmp_attrib;
        }

        virtual void allocateUBOS()
        {
            
        }

        virtual void pushAllUBOS()
        {

        }


        const std::vector<VkPipelineShaderStageCreateInfo>&
            getShaderStages()
        {
            return this->stages;
        }

        std::string getShaderName() { return this->name; }

    private:
        static std::map<std::string, va_shader*> ALL_SHADERS_USED;
        typedef struct
        {
            const char* format;
            VkShaderStageFlagBits bit;
        } ShaderType;
        
        const ShaderType SHADER_TYPES[NUM_SHADER_TYPES] = {
            {".vert", VK_SHADER_STAGE_VERTEX_BIT}, 
            {".frag", VK_SHADER_STAGE_FRAGMENT_BIT}, 
            {".geom", VK_SHADER_STAGE_GEOMETRY_BIT},
            {".tcs",  VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
            {".tes",  VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        };

    protected:
        const std::string name;
        const VkDevice& device;

        const std::vector<VkPipelineShaderStageCreateInfo> stages;
        

        std::vector<VkPipelineShaderStageCreateInfo> readStages(
            std::vector<std::string> files, VkDevice& device);
        
        std::vector<char>* readFile(const std::string& filename);

        char* extractName(const char* fullname);
    };
}
