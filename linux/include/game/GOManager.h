#pragma once

#include "../general/IDependent.h"
#include "../general/IUncopiable.h"
#include "../vulkan/vModelManager.h"
#include "IGameObject.h"

namespace PL
{
    class GOManager : public IUncopiable, public IDependent
    {
    public:
        const static std::string _DEP_ID;
        std::vector<std::string> GetNeededDependencies()
        {
            return {vModelManager::_DEP_ID};
        }
        void ReceiveContext(std::vector<std::vector<IDependent*>> context)
        {                    
            this->modelManager = static_cast<vModelManager*>(context[0][0]);
            this->Initialize();
        }
        void UpdateContext(std::vector<std::vector<IDependent*>> context)
        {
            
        }

        std::string GetDependencyID()
        {
            return this->_DEP_ID;
        }


        const std::vector<IGameObject*> GetActiveGOs()
        {

        }

        void CreateGameObject(std::string type)
        {

        }

    protected:
        vModelManager* modelManager;
        //std::vector<> all;
        

        void Initialize()
        {

        }


    };
}