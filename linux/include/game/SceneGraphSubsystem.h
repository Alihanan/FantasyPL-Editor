#pragma once

#include "../general/IUncopiable.h"
#include "../vulkan/RenderSubsystem.h"
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
            // TODO
        }

        void Update()
        {
            this->UpdateGO(this->sceneGameObject);
        }

        void RenderAll(RenderSubsystem* renderer)
        {
            this->sceneGameObject->Render(renderer);
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