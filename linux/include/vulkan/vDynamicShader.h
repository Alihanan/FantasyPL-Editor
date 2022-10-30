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
            glm::vec3 inPosition;
        };
        const std::vector<VkVertexInputBindingDescription> binding = 
           {{0, sizeof(normal_Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};

        const std::vector<VkVertexInputBindingDescription>&
            getBindingDescriptions()
        {
            return binding;
        }
        const std::vector<VkVertexInputAttributeDescription> attrib = {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(normal_Vertex, inPosition)},
        };
        const std::vector<VkVertexInputAttributeDescription>&
            getAttributeDescriptions()
        {
            return attrib;
        }
        const std::vector<VkPushConstantRange> pushConstRanges = {
           { VK_SHADER_STAGE_VERTEX_BIT , 0, sizeof(matrix_ubo)},
        };
        const std::vector<VkPushConstantRange>& getPushConstantRanges() {
           return pushConstRanges;
        };
        struct matrix_ubo{
            alignas(16) glm::mat4 modelViewProj;
        };
        matrix_ubo matrix;

        matrix_ubo get_matrix()
         { return this->matrix; }

        void set_matrix(matrix_ubo in) 
        {
           this->matrix = in;
        }
        void pushUBOS(VkCommandBuffer buffer) {
            vkCmdPushConstants(buffer, this->layout, 
                VK_SHADER_STAGE_VERTEX_BIT ,
                0, sizeof(matrix_ubo),
                &this->matrix
            );
        }
    };
}
// Dynamically generated from shaders:
//    ./shaders/terrain.tesc ./shaders/terrain.vert ./shaders/terrain.frag ./shaders/terrain.tese 
namespace PL{
    class vShader_terrain : public vShader
    {
    public:
        vShader_terrain(vDevice* device)
            : vShader({
            "./shaders/terrain.tesc","./shaders/terrain.vert","./shaders/terrain.frag","./shaders/terrain.tese",
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
        const std::vector<VkPushConstantRange> pushConstRanges = {
           { VK_SHADER_STAGE_VERTEX_BIT , 0, sizeof(matrix_ubo)},
           { VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT , 0, sizeof(ubo_ubo)},
        };
        const std::vector<VkPushConstantRange>& getPushConstantRanges() {
           return pushConstRanges;
        };
        struct matrix_ubo{
            alignas(16) glm::mat4 modelViewProj;
        };
        matrix_ubo matrix;

        matrix_ubo get_matrix()
         { return this->matrix; }

        void set_matrix(matrix_ubo in) 
        {
           this->matrix = in;
        }        struct ubo_ubo{
            alignas(16) glm::mat4 modelViewProj;
        };
        ubo_ubo ubo;

        ubo_ubo get_ubo()
         { return this->ubo; }

        void set_ubo(ubo_ubo in) 
        {
           this->ubo = in;
        }
        void pushUBOS(VkCommandBuffer buffer) {
            vkCmdPushConstants(buffer, this->layout, 
                VK_SHADER_STAGE_VERTEX_BIT ,
                0, sizeof(matrix_ubo),
                &this->matrix
            );
            vkCmdPushConstants(buffer, this->layout, 
                VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ,
                0, sizeof(ubo_ubo),
                &this->ubo
            );
        }
    };
}
