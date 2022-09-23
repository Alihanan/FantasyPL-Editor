#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vMemoryManager.h"
#include "vPipeConfig.h"
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
            vMemoryManager::_DEP_ID, vPipeConfig::_DEP_ID
        };
        std::vector<std::string> GetNeededDependencies()
        {
            return this->_DEP_NEEDED_DEPS;
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->memoryManager = static_cast<vMemoryManager*>(context[0][0]);
            this->pipeConfig = static_cast<vPipeConfig*>(context[1][0]);

        }

        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }

        ~vModelManager();
        void readAllModelsFromJSON(std::string jsonFileName);

    protected:
        vMemoryManager* memoryManager;
        vPipeConfig* pipeConfig;

        std::map<std::string, vPipeline*> createdPipelines;

    };
}