#pragma once

#include "IGameObject.h"

namespace PL
{
    class TerrainChunkGO : public IGameObject
    {
    public:
        TerrainChunkGO(IGameObject* terrainGO) : IGameObject(terrainGO)
        {
            GameModel* gm = new GameModel();
            gm->Type(TERRAIN_MODEL);

            InputVertType* type = new InputVertType;
            *type = TRIANGLE_VERT_IN;

            gm->Param(PARAM_HEIGHTMAP, new std::string("heightmap_x0_y1.png"));
            gm->Param(PARAM_INPUT_VERT_TYPE, type);
            gm->Param(PARAM_SHADER_FILE, new std::string("normal"));
            gm->Param(PARAM_TRANSFORM, &(this->transform));
            this->renderModels = {
                gm
            };
        }

        virtual ~TerrainChunkGO()
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