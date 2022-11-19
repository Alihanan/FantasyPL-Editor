#pragma once

#include "IGameObject.h"
#include "TerrainGO.h"

namespace PL
{
    class SceneGO : public IGameObject
    {
    public:
        SceneGO() : IGameObject(nullptr)
        {
            this->children = {new TerrainGO(this)};
        }

        virtual ~SceneGO()
        {
            
        }

        virtual void OnUpdate()
        {
            
        }
        virtual void OnStart()
        {

        }

    protected:
        
    };
}