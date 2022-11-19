#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace PL
{
    enum ParameterID
    {
        PARAM_TRANSFORM, PARAM_OBJ_FILE, PARAM_SHADER_FILE, PARAM_INPUT_VERT_TYPE, PARAM_HEIGHTMAP
    };
    inline static const std::map<ParameterID, std::string> ALL_PARAMETER_NAMES = 
    {
        {PARAM_TRANSFORM, "Transform"},
        {PARAM_OBJ_FILE, "OBJ input file"},
        {PARAM_SHADER_FILE, "Shader file"},
        {PARAM_INPUT_VERT_TYPE, "Input primitive type"},
        {PARAM_HEIGHTMAP, "Heightmap file"}
    };

    enum InputVertType
    {
        TRIANGLE_VERT_IN, QUAD_VERT_IN
    };

    enum ModelTypeID
    {
        NO_MODEL, TERRAIN_MODEL
    };
    struct ModelType
    {
        ModelTypeID id;
        std::string name;
        std::vector<ParameterID> requiredParams;
    };

    inline static const std::map<ModelTypeID, ModelType> ALL_MODEL_TYPES = {

        {TERRAIN_MODEL, 
            {TERRAIN_MODEL, "Terrain Model", {PARAM_TRANSFORM, PARAM_SHADER_FILE, PARAM_INPUT_VERT_TYPE, PARAM_HEIGHTMAP}}
        },

        {NO_MODEL, 
            {NO_MODEL, "No Model", {}}
        }
    };

    class GameModel
    {
    public:
        GameModel() {}
        ~GameModel() {
            for(auto& [key, value] : this->params)
            {

            }
        }

        uint32_t Ord() { return this->ordering;}
        void Ord(uint32_t ord) { this->ordering = ord;}

        ModelType Type() {return this->type;}
        void Type(ModelTypeID ntype) 
        {
            this->type = ALL_MODEL_TYPES.at(ntype);
            this->params.clear();
            for(ParameterID pid : this->type.requiredParams)
            {
                this->params[pid] = nullptr;
            }
        }
        void Param(ParameterID pid, void* value) { 
            if(this->params.count(pid) == 0) { 
                throw std::runtime_error("Trying to set parameter not suitable for this type of model!");
            }
            this->params[pid] = value;
        }

        void* Param(ParameterID pid) {
            if(this->params.count(pid) == 0) { 
                throw std::runtime_error("Trying to get parameter not suitable for this type of model!");
            }
            return this->params[pid];
        };

    protected:
        ModelType type = ALL_MODEL_TYPES.at(NO_MODEL);
        std::map<ParameterID, void*> params = {};
        uint32_t ordering = 0;
    };
}