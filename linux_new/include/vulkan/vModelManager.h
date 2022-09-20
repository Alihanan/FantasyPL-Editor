#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vMemoryManager.h"
#include "vShaderManager.h"
#include <optional>


namespace PL
{
    class vModelManager : public IUncopiable, public IDependent
    {
    public:
        vModelManager() {}
        // IDependent
        const static std::string _DEP_ID;
        inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            vMemoryManager::_DEP_ID, vShaderManager::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->memoryManager = static_cast<vMemoryManager*>(context[0][0]);
            this->shaderManager = static_cast<vShaderManager*>(context[1][0]);
        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        ~vModelManager();
    protected:
        vMemoryManager* memoryManager;
        vShaderManager* shaderManager;
    };
}