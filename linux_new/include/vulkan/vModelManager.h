#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vMemoryManager.h"
#include "vPipeConfig.h"
#include "../io/PseudoJson.h"

#include <optional>


namespace PL
{
    class vModelManager : public IUncopiable, public IDependent
    {
    public:
        vModelManager() {}
        // IDependent
        const static std::string _DEP_ID;
        // inline const static std::vector<std::string> _DEP_NEEDED_DEPS = {
            
        // };
        std::vector<std::string> GetNeededDependencies()
        {
            return {vMemoryManager::_DEP_ID, vPipeConfig::_DEP_ID};
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {          
            this->memoryManager = static_cast<vMemoryManager*>(context[0][0]);
            this->pipeConfig = static_cast<vPipeConfig*>(context[1][0]);
            this->Initialize();
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

        void RecordAllPipelines(VkCommandBuffer& currentBuffer);

    protected:
        vMemoryManager* memoryManager;
        vPipeConfig* pipeConfig;

        void Initialize();
        void readShaderInfo(PJSON settings);

        std::map<std::string, vPipeline*> createdPipelines;

    };
    inline const std::string vModelManager::_DEP_ID = IDependent::type(vModelManager());//typeid(*(new vPipeline())).name();
}