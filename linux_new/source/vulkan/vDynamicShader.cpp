#include "../../include/vulkan/vDynamicShader.h"


namespace PL {
    vShader* vShader::createShader(std::string name, vDevice* device)
    {
        if(vShader::ALL_SHADERS_USED.find(name) != vShader::ALL_SHADERS_USED.end())
        {
            return vShader::ALL_SHADERS_USED[name];
        }
        // START OF FACTORY

        if(name == std::string("normal")) {
            vShader::ALL_SHADERS_USED[name] = new vShader_normal(device);
        }

        if(name == std::string("terrain")) {
            vShader::ALL_SHADERS_USED[name] = new vShader_terrain(device);
        }

        // END OF FACTORY
        if(vShader::ALL_SHADERS_USED.find(name) != vShader::ALL_SHADERS_USED.end())
        {
            return vShader::ALL_SHADERS_USED[name];
        }

        return nullptr;
    }
}

