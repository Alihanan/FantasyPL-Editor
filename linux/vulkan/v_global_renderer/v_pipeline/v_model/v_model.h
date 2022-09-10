#pragma once
#include "../va_uncopiable/va_uncopiable.h"
#include "../v_device/v_device.h"

namespace PL
{
    class v_pipeline;
    /*
    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;
        
        static const std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        
        static const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() 
        {       
            return bindingDescriptions;
        }

        static const std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        
        static const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() 
        {
            return attributeDescriptions;
        }
    };*/
    
    /*
    static const std::vector<Vertex> triangle_vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };*/
    
    class v_model : va_uncopiable
    {
    public:
        v_model(v_device* device, VkRenderPass& pass, std::string sourceFile, std::string shaderName);
        ~v_model();

        void draw(VkCommandBuffer comBuf);
        void bind(VkCommandBuffer comBuf);
    protected:
        //void createBuffers(const std::vector<PL::Vertex>& vertices, uint32_t num_vertices);
        void createBuffers(const void* vertices, uint32_t size);
        
        v_device* device;
        v_pipeline* pipeline;
        Buffer buffer;
        uint32_t vertex_count;
    };
    
}