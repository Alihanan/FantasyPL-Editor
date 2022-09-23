#pragma once

#include "../general/IUncopiable.h"

#include "vMemoryManager.h"
#include "vShader.h"
#include <optional>


namespace PL
{
    class vModel : public IUncopiable
    {
    public:       
        // Others
        vModel(std::string file, vMemoryManager* manager, vShader* shader) :
            memoryManager(manager), shader(shader), fileName(file)
        {
            this->memoryManager->AllocateVBOandUBO(this);
        }


        ~vModel();
        void bind(VkCommandBuffer comBuf);
        void draw(VkCommandBuffer comBuf);

        std::string GetSourceDataFileName() {return this->fileName; } 

        vMemoryManager::Data vModel::processData();
        
    protected:
        vMemoryManager* memoryManager;
        vShader* shader;

        uint32_t num_vertices = 0;

    private:
        vModel() {}
        std::string fileName;


        
    };


}