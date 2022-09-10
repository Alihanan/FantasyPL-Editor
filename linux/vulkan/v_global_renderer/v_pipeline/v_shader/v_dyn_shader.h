#pragma once
#include "va_shader.h"


// Dynamically generated from shaders:
//    ./shaders/normal.frag ./shaders/normal.vert 
namespace PL{
    class v_normal_shader : public va_shader
    {
    public:
        v_normal_shader(VkDevice& device)
            : va_shader({
            "./shaders/normal.frag","./shaders/normal.vert",
            }, device)
        {}
        ~v_normal_shader() {}
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
        struct normal_UniformBufferObject_ubo{
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
        normal_UniformBufferObject_ubo UniformBufferObject_ubo;

        normal_UniformBufferObject_ubo get_ubo_UniformBufferObject()
         { return this->UniformBufferObject_ubo; }

        void set_ubo_UniformBufferObject(normal_UniformBufferObject_ubo in) 
        { this->UniformBufferObject_ubo = in; } // TODO add pushing to pipeline etc 

        void pushUBOS() {} 

        void allocateUBOS() {} 

    };
}
// Dynamically generated from shaders:
//    ./shaders/simple.frag ./shaders/simple.vert 
namespace PL{
    class v_simple_shader : public va_shader
    {
    public:
        v_simple_shader(VkDevice& device)
            : va_shader({
            "./shaders/simple.frag","./shaders/simple.vert",
            }, device)
        {}
        ~v_simple_shader() {}
        struct simple_Vertex {
        };
        const std::vector<VkVertexInputBindingDescription> binding = 
           {{0, sizeof(simple_Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};

        const std::vector<VkVertexInputBindingDescription>&
            getBindingDescriptions()
        {
            return binding;
        }
        const std::vector<VkVertexInputAttributeDescription> attrib = {
        };
        const std::vector<VkVertexInputAttributeDescription>&
            getAttributeDescriptions()
        {
            return attrib;
        }

        void pushUBOS() {} 

        void allocateUBOS() {} 

    };
}
// Dynamically generated from shaders:
//    ./shaders/terrain.frag ./shaders/terrain.tese ./shaders/terrain.vert 
namespace PL{
    class v_terrain_shader : public va_shader
    {
    public:
        v_terrain_shader(VkDevice& device)
            : va_shader({
            "./shaders/terrain.frag","./shaders/terrain.tese","./shaders/terrain.vert",
            }, device)
        {}
        ~v_terrain_shader() {}
        struct terrain_Vertex {
            glm::vec3 inLocalPosition;
            glm::vec3 inNormal;
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
                {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(terrain_Vertex, inNormal)},
        };
        const std::vector<VkVertexInputAttributeDescription>&
            getAttributeDescriptions()
        {
            return attrib;
        }
        struct terrain_UniformBufferObject_ubo{
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
        terrain_UniformBufferObject_ubo UniformBufferObject_ubo;

        terrain_UniformBufferObject_ubo get_ubo_UniformBufferObject()
         { return this->UniformBufferObject_ubo; }

        void set_ubo_UniformBufferObject(terrain_UniformBufferObject_ubo in) 
        { this->UniformBufferObject_ubo = in; } // TODO add pushing to pipeline etc 
        struct terrain_UniformBufferObject_ubo{
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
        terrain_UniformBufferObject_ubo UniformBufferObject_ubo;

        terrain_UniformBufferObject_ubo get_ubo_UniformBufferObject()
         { return this->UniformBufferObject_ubo; }

        void set_ubo_UniformBufferObject(terrain_UniformBufferObject_ubo in) 
        { this->UniformBufferObject_ubo = in; } // TODO add pushing to pipeline etc 

        void pushUBOS() {} 

        void allocateUBOS() {} 

    };
}
