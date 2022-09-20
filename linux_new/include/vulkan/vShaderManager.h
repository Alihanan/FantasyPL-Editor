#pragma once
#include "../general/IUncopiable.h"
#include "../general/IDependent.h"
#include "vDevice.h"
#include "vShader.h"


// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <map>
#include <string>

namespace PL
{
    class vShaderManager : public IUncopiable, public IDependent
    {
    public:
        vShaderManager() {}

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

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }


        // Other members
        ~vShaderManager();

        vShader* createShader(std::string shaderName);

    protected:
        std::map<std::string, vShader*> allShaders;

        vDevice* device;
        void Initialize();

    };
}