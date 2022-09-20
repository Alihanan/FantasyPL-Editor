#include "../../include/vulkan/vModel.h"
#include "../../include/math/algorithms.h"
#include "../../include/io/PseudoJson.h"

namespace PL
{

    vModel::~vModel()
    {
        this->memoryManager->FreeVBOandUBO(this);
    }
    


    void vModel::bind(VkCommandBuffer comBuf)
    {
        VkBuffer& buffer = this->memoryManager->GetOrAllocateVBOandUBO(this);
        VkBuffer vertexBuffers[] = {buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(comBuf, 0, 1, vertexBuffers, offsets);
    }

    void vModel::draw(VkCommandBuffer comBuf)
    {
        vkCmdDraw(comBuf, this->num_vertices, 1, 0, 0);
    }

    void* vModel::processFile(std::string fileName)
    {
        PJSON pjson = PseudoJson::readFile(fileName);

        if(!pjson.hasKey("models")) 
            throw std::runtime_error("Wrong JSON format for model definition!");

        auto& models = pjson["models"];

        for( auto &j : models.ArrayRange())
        {
            if(!j.hasKey("type") || !j.hasKey("src"))
            {
                throw std::runtime_error("Wrong JSON format for model definition!");
            }

            std::string type = j["type"].ToString();
            std::string srcFile = j["src"].ToString();
            
            std::cout << type << ", " << srcFile << std::endl;
            return nullptr;
        }

        return nullptr;
    }

    void* vModel::generateGrid(uint32_t WIDTH, uint32_t HEIGH)
    {
        return nullptr;
    }
}