#pragma once

#include "IGameObject.h"
#include <iostream>

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
            this->transform.translate(0.001f, 0.0f, 0.0f);
            if(this->transform.tr_x() > 0.5f)
            {
                this->transform.set_position(0.1f, this->transform.tr_y(), 0.0f);
            }
        }
        virtual void OnStart()
        {

        }

    protected:
        

    };
}