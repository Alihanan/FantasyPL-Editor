#include "../../include/vulkan/vMemoryManager.h"

#include "../../include/vulkan/vModel.h"
#include <cstring>

namespace PL
{
    const std::string vMemoryManager::_DEP_ID = IDependent::type(vMemoryManager());

    vMemoryManager::~vMemoryManager()
    {

    }

    void vMemoryManager::Initialize()
    {

    }
    void vMemoryManager::AllocateVBOandUBO(vModel* model)
    {
        if(this->currentMemory.find(model) != this->currentMemory.end())
        {
            return;
        }
        MemoryCell* cell = new MemoryCell;
        cell->buffer = this->createBuffer(model);
        //cell->lastUsedTime = 
        this->currentMemory[model] = cell;
    }
    VkBuffer& vMemoryManager::GetOrAllocateVBOandUBO(vModel* model)
    {
        if(this->currentMemory.find(model) == this->currentMemory.end())
        {
            this->AllocateVBOandUBO(model);
        }
        return this->currentMemory[model]->buffer->buffer;
    }
    void vMemoryManager::FreeVBOandUBO(vModel* model)
    {
        if(this->currentMemory.find(model) == this->currentMemory.end())
        {
            return;
        }
        MemoryCell* cell = this->currentMemory[model];
        Buffer* buf = cell->buffer;
        delete cell;
        this->currentMemory.erase(model);
        this->freeBuffer(buf);
    }
    vMemoryManager::Buffer* vMemoryManager::createBuffer(vModel* model)
    {
        Data data = model->processData();
        return this->generateBuffer(data.data, data.size);
    }

    vMemoryManager::Data readData(std::string file)
    {
        

        return {nullptr, 0};
    }

    vMemoryManager::Buffer* vMemoryManager::generateBuffer(void* data, uint32_t size)
    {
        Buffer* buf = new Buffer;

        VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // VB buffer
        VkMemoryPropertyFlags properties =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // write accessible
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;  // flush auto
        

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;//VBO
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //simultaneous use from diff. queue families

        if (vkCreateBuffer(this->device->GetReadyDevice()->logicalDevice,
                &bufferInfo, nullptr, &(buf->buffer)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }


        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->device->GetReadyDevice()->logicalDevice, 
                                    buf->buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = 
            this->device->findMemoryType(memRequirements.memoryTypeBits, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


        if (vkAllocateMemory(this->device->GetReadyDevice()->logicalDevice, &allocInfo, nullptr, 
                &(buf->memory)) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(this->device->GetReadyDevice()->logicalDevice, buf->buffer, buf->memory, 0);

        void* tmpdata;
        vkMapMemory(this->device->GetReadyDevice()->logicalDevice, 
            buf->memory, 0, buf->buf_size, 0, &tmpdata);
        
        memcpy(tmpdata, data, (size_t) buf->buf_size);

        vkUnmapMemory(this->device->GetReadyDevice()->logicalDevice, buf->memory);

        delete data;

        return buf;
    }

    void vMemoryManager::freeBuffer(Buffer* buf)
    {
        vkDestroyBuffer(this->device->GetReadyDevice()->logicalDevice, buf->buffer, nullptr);
        vkFreeMemory(this->device->GetReadyDevice()->logicalDevice, buf->memory, nullptr);
        delete buf;
    }
}