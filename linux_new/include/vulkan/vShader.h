#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
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
    class vShader : public IUncopiable, public IDependent
    {
    public:
        inline static const std::string SHADER_SPIRV_FORMAT = ".spv";
        inline static const std::string SHADER_DEFAULT_PATH = "shaders/";
        static vShader* createShader(std::string name);

        typedef struct _struct_shader_stages
        {
            const char* name;
            const char* format;
            VkShaderStageFlagBits bit;
        } ShaderStage;

        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vDevice::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->device = static_cast<vDevice*>(context[0][0]);
            this->Initialize();
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            auto ret = std::string(typeid(this).name());
        }
        
        bool IsSingleton()
        {
            return true;
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }
        
        // Others
    protected:
        vShader(std::vector<std::string> files);
    public:
        ~vShader();

    protected:
        void Initialize();

        vDevice* device = nullptr;

    private:
        static std::map<std::string, vShader*> ALL_SHADERS_USED;
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