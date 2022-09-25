#pragma once

#include "../general/IUncopiable.h"
#include "../io/PseudoJson.h"
#include "vMemoryManager.h"
#include "vPipeConfig.h"
#include "vShader.h"


#include <optional>


namespace PL
{
    class vPipeConfig;
    
    class vModel : public IUncopiable
    {
    public:       
        static vModel* createModelFactory(PJSON settings, vMemoryManager* manager, vShader* shader);

        virtual ~vModel();
        virtual void bind(VkCommandBuffer comBuf);
        virtual void draw(VkCommandBuffer comBuf);
        virtual vMemoryManager::Data processData() = 0;
        
    protected:
        // Others
        vModel(vMemoryManager* manager, vShader* shader) :
            memoryManager(manager), shader(shader)
        {
            this->num_vertices = 0;
        }
        vMemoryManager* memoryManager;
        vShader* shader;

        uint32_t num_vertices = 0;

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
            filename(file), vModel(manager, shader) 
        { 
            
        }
        virtual vMemoryManager::Data processData();

    protected:
        std::string filename;
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
        /**
         *   After constructor we should pass the settings from the JSON about height files
         */
        void SetHeightMaps(std::vector<std::string> filenames);
        void SetResolution(uint32_t W, uint32_t H);

        virtual vMemoryManager::Data processData();
        

    protected:
        struct HeightMapChunk
        {
            std::string dataFile = "";
            uint32_t width_coord = 0;
            uint32_t height_coord = 0;
        };
        std::vector<HeightMapChunk> heightChunks;

        uint32_t current_res_W = 10;
        uint32_t current_res_H = 10;

        float chunk_size_W = 1.0f;
        float chunk_size_H = 1.0f;
    };
}