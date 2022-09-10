#include "v_dyn_shader.h"


namespace PL {
    va_shader* va_shader::createShader(std::string name, VkDevice& device)
    {
        if(va_shader::ALL_SHADERS_USED.find(name) != va_shader::ALL_SHADERS_USED.end())
        {
            return va_shader::ALL_SHADERS_USED[name];
        }

        if(name == std::string("normal")) {
            va_shader::ALL_SHADERS_USED[name] = new v_normal_shader(device);
        }

        if(name == std::string("simple")) {
            va_shader::ALL_SHADERS_USED[name] = new v_simple_shader(device);
        }

        if(name == std::string("terrain")) {
            va_shader::ALL_SHADERS_USED[name] = new v_terrain_shader(device);
        }

        if(va_shader::ALL_SHADERS_USED.find(name) != va_shader::ALL_SHADERS_USED.end())
        {
            return va_shader::ALL_SHADERS_USED[name];
        }

        return nullptr;
    }
}

