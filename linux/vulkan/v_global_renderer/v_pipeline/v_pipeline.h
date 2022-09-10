#pragma once
#include "../va_uncopiable/va_uncopiable.h"
#include "../v_device/v_device.h"
#include "../v_swapchain/v_swapchain.h"
#include "v_pipeline_config.h"

#include <vector>

namespace PL
{
    class v_model;
    /*
    typedef struct _struct_shader_type
    {
        std::string name;
        std::string format;
        bool isNeeded;
        VkShaderStageFlagBits bit;
    } ShaderType;
    static const ShaderType SHADER_TYPES[] = {
        {"vertex shader",   ".vert", true, VK_SHADER_STAGE_VERTEX_BIT}, 
        {"fragment shader", ".frag", true, VK_SHADER_STAGE_FRAGMENT_BIT}, 
        {"geometry shader", ".geom", false, VK_SHADER_STAGE_GEOMETRY_BIT}
    };
    static const uint32_t SHADER_TYPE_SIZE = sizeof(SHADER_TYPES)/sizeof(SHADER_TYPES[0]);
    typedef struct _struct_shader_data
    {
        std::vector<char>* data;
        ShaderType type;
    } ShaderData;

    typedef struct _struct_shader_obj
    {
        VkPipelineShaderStageCreateInfo infos[SHADER_TYPE_SIZE];
        bool isIncluded[SHADER_TYPE_SIZE];
    } ShaderObj;    */

    class v_pipeline : va_uncopiable
    {
    public:
        

        v_pipeline(v_device* device, VkRenderPass& renderPass, std::string shaderName);
        
        ~v_pipeline();

        void addModel(v_model* model)        
        { 
            this->models.push_back(model);
            //this->models.clear();
        }
        void recordPipeline(v_swapchain* swapchain, uint32_t bufferIndex, uint32_t imageIndex);
        
        void beginRecordingCommandBuffer(v_swapchain* swapchain, uint32_t bufferIndex, uint32_t imageIndex);
        void endRecordingCommandBuffer(v_swapchain* swapchain, uint32_t bufferIndex, uint32_t imageIndex);
        void recreateCommandBuffers(uint32_t size);

        std::vector<VkCommandBuffer>& getCommandBuffer() {return this->commandBuffers;}
        void updateDynamicStates(uint32_t w, uint32_t h);

        void setViewport(float x, float y, float w, float h);
        void setScissor(int32_t x_offset, int32_t y_offset, uint32_t w, uint32_t h);
        void setDepthScissor(float minD, float maxD);
        
        VkViewport& getDynamicViewport(){return this->viewport;}
        VkRect2D& getDynamicScissor(){return this->scissor;}

    protected:       
        
        //v_swapchain* swapchain;

        //ShaderObj** allShaders;
        v_pipeline_config* config;
        std::string shader_name;
        VkRenderPass& renderPass;
        v_device* device;
        std::vector<v_model*> models;

        void initialize();
        //void createShaders();

        VkViewport viewport{};
        VkRect2D scissor{};

        
        VkPipeline graphicsPipeline = VK_NULL_HANDLE;
        void createPipeline();

        VkCommandPool commandPool;
        void createCommandPools();

        std::vector<VkCommandBuffer> commandBuffers;
        void allocateCommandBuffers(uint32_t size);
        void freeCommandBuffers();

        /*
        ShaderObj* createShaderModule(std::string name);
        ShaderData* readShaders(std::string name);
        std::vector<char>* readFile(const std::string& filename);*/
    };



}