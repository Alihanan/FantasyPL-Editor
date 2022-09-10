#include "v_model.h"
#include "../v_pipeline/v_pipeline.h"

namespace PL{


v_model::v_model(v_device* device, VkRenderPass& pass, std::string sourceFile, std::string shaderName)
: device(device), pipeline(v_pipeline::createPipeline(device, pass, shaderName))
{
    this->pipeline->addModel(this);
    

    // TODO -> read from shader
    //this->vertex_count = static_cast<uint32_t>(triangle_vertices.size());
    //this->createBuffers(triangle_vertices, 
    //            this->vertex_count);
}

v_model::~v_model()
{
    this->device->freeBuffer(&this->buffer);
}

void v_model::draw(VkCommandBuffer comBuf)
{
    // ~ DrawArrays()
    vkCmdDraw(comBuf, this->vertex_count, 1, 0, 0);
}
void v_model::bind(VkCommandBuffer comBuf)
{
    // OpenGL -> something like bind VAO
    // only 1 VAO/VBO used
    
    VkBuffer vertexBuffers[] = {this->buffer.buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(comBuf, 0, 1, vertexBuffers, offsets);
}
void v_model::createBuffers(const void* vertices, uint32_t num_vertices)
{}
/*
void v_model::createBuffers(const std::vector<PL::Vertex>& vertices, uint32_t num_vertices)
{
    if(num_vertices == 0)
    {
        throw std::runtime_error("Empty model created!");
    }

    if(num_vertices < 3)
    {
        throw std::runtime_error("Too small model created! Minimum vertices are 3 -> triangle");
    }


    VkDeviceSize size = sizeof(vertices[0]) * num_vertices;
    
    this->device->allocateBuffer(size, 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, // VB buffer
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // write accessible
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,  // flush auto
        &(this->buffer)
    );

    this->device->fillBuffer(&this->buffer, vertices.data());
}*/

}