#pragma once

#include "../general/IUncopiable.h"
#include "IGameObject.h"
#include "SceneGO.h"
#include "ModelDefines.h"

#include <vector>


namespace PL
{
    class SceneGraphSubsystem : public IUncopiable
    {
    public:

        SceneGraphSubsystem()
        {
            this->Initialize();
        }


        ~SceneGraphSubsystem()
        {
            delete sceneGameObject;
        }

        void CreateGameObject(std::string type)
        {
            
        }

        void Update()
        {
            this->UpdateGO(this->sceneGameObject);
        }

        std::vector<GameModel*>* GetAllRenderObjects()
        {
            auto* allModels = new std::vector<GameModel*>();
            this->sceneGameObject->GetModel(allModels);
            return allModels;
        }


    protected:
        SceneGO* sceneGameObject;

        void Initialize()
        {
            sceneGameObject = new SceneGO();
        }

        void UpdateGO(IGameObject* go)
        {
            go->OnUpdate();
            for(IGameObject* child : go->GetChildrenGO())
            {
                this->UpdateGO(child);
            }
        }

    };
}