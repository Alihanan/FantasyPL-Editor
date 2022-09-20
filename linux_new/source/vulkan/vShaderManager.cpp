#include "../../include/vulkan/vShaderManager.h"


namespace PL
{
    const std::string vShaderManager::_DEP_ID = IDependent::type(vShaderManager());

    vShader* vShaderManager::createShader(std::string shaderName)
    {
        if(allShaders.find(shaderName) != allShaders.end())
        {
            return allShaders[shaderName];
        }
        auto shader = vShader::createShader(shaderName, this->device);

        allShaders[shaderName] = shader;

        return shader;
    }

    vShaderManager::~vShaderManager()
    {
        for(const auto& [key, value] : allShaders)
        {
            delete value;
        }
    }

    void vShaderManager::Initialize()
    {
        
    }
}