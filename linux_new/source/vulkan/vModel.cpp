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

    vMemoryManager::Data vModel::processData()
    {
        return {nullptr, 0};
    }
}