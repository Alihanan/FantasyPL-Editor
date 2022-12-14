#pragma once

#include "../general/IUncopiable.h"
#include "vMemoryManager.h"
#include "vPipeConfig.h"
#include "vShader.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

#include <optional>
#include <map>

#include "../game/ModelDefines.h"

namespace PL
{
    class vPipeConfig;
    
    class vModel : public IUncopiable
    {
    protected:
        inline static std::map<GameModel*, vModel*> activeModels = {};
    public:       
        static vModel* createModelFactory(GameModel* type, vMemoryManager* manager, vShader* shader);

        virtual ~vModel();
        virtual void setParams(GameModel* paramHolder);

        virtual void bind(VkCommandBuffer& comBuf);
        virtual void draw(VkCommandBuffer& comBuf);
        virtual void setUniforms();

        virtual uint32_t getNumberVertices() = 0;
        virtual vMemoryManager::Data processData() = 0;
        
    protected:
        // Others
        vModel(vMemoryManager* manager, vShader* shader) :
            memoryManager(manager), shader(shader)
        {
            
        }
        vMemoryManager* memoryManager;
        vShader* shader;
        
    private:
        vModel() {}
    };
}
namespace PL
{
    class vFileModel : public vModel
    {
    public:
        vFileModel(std::string file, vMemoryManager* manager, vShader* shader) :
            filename(file), num_vertices(0), vModel(manager, shader) 
        { 
            
        }
        vMemoryManager::Data processData();
        
        uint32_t getNumberVertices() {return this->num_vertices;}
    protected:
        std::string filename = "";
        uint32_t num_vertices = 0;
    };
}
namespace PL
{
    class vTerrainModel : public vModel
    {
    public:
        vTerrainModel(vMemoryManager* manager, vShader* shader) :
            vModel(manager, shader) 
        {
            
        }
        virtual void setParams(GameModel* paramHolder) override;
        /**
         *   After constructor we should pass the settings from the JSON about height files
         */
        void SetHeightMap(std::string filename);
        void SetResolution(uint32_t W, uint32_t H);
        uint32_t getNumberVertices() {return this->current_res_W * this->current_res_H * quad_num;}
        vMemoryManager::Data processData();
        void setUniforms();

    protected:
        std::string dataFile = "";
        uint32_t width_coord = 0;   // position in map ??? 
        uint32_t height_coord = 0;  // position in map ???

        uint32_t quad_num = 6;

        uint32_t current_res_W = 100;
        uint32_t current_res_H = 100;

        float chunk_size_W = 0.005f;
        float chunk_size_H = 0.005f;
    };
}