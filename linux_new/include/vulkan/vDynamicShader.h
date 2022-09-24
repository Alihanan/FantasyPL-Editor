#pragma once
#include "vShader.h"


// Dynamically generated from shaders:
//    ./shaders/normal.frag ./shaders/normal.vert 
namespace PL{
    class vShader_normal : public vShader
    {
    public:
        vShader_normal(vDevice* device)
            : vShader({
            "./shaders/normal.frag","./shaders/normal.vert",
            }, device)
        {}
        ~vShader_normal() {}
        struct normal_Vertex {
            glm::vec2 inPosition;
            glm::vec3 inColor;
        };
        const std::vector<VkVertexInputBindingDescription> binding = 
           {{0, sizeof(normal_Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};

        const std::vector<VkVertexInputBindingDescription>&
            getBindingDescriptions()
        {
            return binding;
        }
        const std::vector<VkVertexInputAttributeDescription> attrib = {
                {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(normal_Vertex, inPosition)},
                {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(normal_Vertex, inColor)},
        };
        const std::vector<VkVertexInputAttributeDescription>&
            getAttributeDescriptions()
        {
            return attrib;
        }
        struct UniformBufferObject_ubo{
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
        UniformBufferObject_ubo UniformBufferObject;

        UniformBufferObject_ubo get_UniformBufferObject()
         { return this->UniformBufferObject; }

        void set_UniformBufferObject(UniformBufferObject_ubo in) 
        { this->UniformBufferObject = in; } // TODO add pushing to pipeline etc 

        void pushUBOS() {} 

        void allocateUBOS() {} 

    };
}
// Dynamically generated from shaders:
//    ./shaders/terrain.vert ./shaders/terrain.frag ./shaders/terrain.tese 
namespace PL{
    class vShader_terrain : public vShader
    {
    public:
        vShader_terrain(vDevice* device)
            : vShader({
            "./shaders/terrain.vert","./shaders/terrain.frag","./shaders/terrain.tese",
            }, device)
        {}
        ~vShader_terrain() {}
        struct terrain_Vertex {
            glm::vec3 inLocalPosition;
        };
        const std::vector<VkVertexInputBindingDescription> binding = 
           {{0, sizeof(terrain_Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};

        const std::vector<VkVertexInputBindingDescription>&
            getBindingDescriptions()
        {
            return binding;
        }
        const std::vector<VkVertexInputAttributeDescription> attrib = {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(terrain_Vertex, inLocalPosition)},
        };
        const std::vector<VkVertexInputAttributeDescription>&
            getAttributeDescriptions()
        {
            return attrib;
        }
        struct UniformBufferObject_ubo{
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
        UniformBufferObject_ubo UniformBufferObject;

        UniformBufferObject_ubo get_UniformBufferObject()
         { return this->UniformBufferObject; }

        void set_UniformBufferObject(UniformBufferObject_ubo in) 
        { this->UniformBufferObject = in; } // TODO add pushing to pipeline etc 

        void pushUBOS() {} 

        void allocateUBOS() {} 

    };
}
