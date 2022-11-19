#pragma once

#include "IGameObject.h"
#include "TerrainChunkGO.h"
namespace PL
{
    class TerrainGO : public IGameObject
    {
    public:
        TerrainGO(IGameObject* scene) : IGameObject(scene)
        {
            // create terrain chunks
            IGameObject* chunk = new TerrainChunkGO(this);
            this->children.push_back(chunk);
            chunk->transf().translate(0.1f, 0.25f, 0.0f);

            chunk = new TerrainChunkGO(this);
            this->children.push_back(chunk);
            chunk->transf().translate(-0.1f, -0.25f, 0.0f);
        }

        virtual ~TerrainGO()
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