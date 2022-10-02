#include "../../include/vulkan/vModel.h"
#include "../../include/vulkan/vPipeline.h"
#include "../../include/math/algorithms.h"
#include "../../include/io/PseudoJson.h"
#include "../../include/vulkan/vDynamicShader.h"

#include "../../include/external/stb_image.hpp"


namespace PL
{
    vModel* vModel::createModelFactory(PJSON settings, vMemoryManager* manager, vShader* shader)
    {
        if(!settings.hasKey("type") || !settings.hasKey("src"))
        {
            throw std::runtime_error("Wrong JSON format for model definition!" + settings.ToString());
        }

        auto& src = settings["src"];
        std::string type = settings["type"].ToString();
        
        vModel* ret;
        if(type == "terrain")
        {
            vTerrainModel* terrain = new vTerrainModel(manager, shader);

            std::vector<std::string> hFiles;
            for( auto &j : src.ArrayRange())
            {
                hFiles.push_back(j.ToString());
            }
            terrain->SetHeightMaps(hFiles);
            ret = terrain;
        }
        else if(type == "obj")
        {
            vFileModel* fModel = new vFileModel(src.ToString(), manager, shader);
            ret = fModel;
        }

        return ret;
    }

    vModel::~vModel()
    {
        this->memoryManager->FreeVBOandUBO(this);
    }

    void vModel::bind(VkCommandBuffer& comBuf)
    {
        VkBuffer& buffer = this->memoryManager->GetOrAllocateVBOandUBO(this);
        VkBuffer vertexBuffers[] = {buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(comBuf, 0, 1, vertexBuffers, offsets);
    }

    void vModel::draw(VkCommandBuffer& comBuf)
    {
        uint32_t number = this->getNumberVertices();
        vkCmdDraw(comBuf, number, 1, 0, 0);
    }

    void vModel::setUniforms()
    {

    }

    vMemoryManager::Data vModel::processData()
    {
        return {nullptr, 0};
    }
}

namespace PL
{
    vMemoryManager::Data vFileModel::processData()
    {
        return {nullptr, 0};
    }
}
namespace PL
{
    void vTerrainModel::SetHeightMaps(std::vector<std::string> filenames)
    {
        for(auto file : filenames)
        {
            auto splitted_name = split_string(file, "_");
            uint32_t x_coord = 0;
            uint32_t y_coord = 0;

            for(auto part : splitted_name)
            {
                if(part[0] == 'x')
                {
                    std::string str = part.erase(0, 1);
                    x_coord = (uint32_t)std::stoi( str );
                    continue;
                }
                if(part[0] == 'y')
                {
                    std::string str = part.erase(0, 1);
                    y_coord = (uint32_t)std::stoi( str );
                    continue;
                }
            }

            HeightMapChunk chunk {};
            chunk.dataFile = file;
            chunk.width_coord = x_coord;
            chunk.height_coord = y_coord;

            this->heightChunks.push_back(chunk);
        }
        
    }

    void vTerrainModel::setUniforms()
    {
        static int i = 0;

        auto tshader = static_cast<vShader_normal*>(this->shader);
        vShader_normal::matrix_ubo to_send{};
        
        to_send.modelViewProj = glm::mat4(1.0f);
        //to_send.modelViewProj = glmEulerRotation(0.0f, 0.0f, 0.0f);
        // to_send.modelViewProj = glm::mat4({
        //     {1.0f, 0.0f, 0.0f, 0.0f}, 
        //     {0.0f, 0.0f, 1.0f, 0.0f},
        //     {0.0f, 1.0f, 0.0f, 0.0f},
        //     {0.0f, 0.0f, 0.0f, 1.0f}});
        to_send.modelViewProj = glm::translate(glm::vec3(0.0f - (i % 1000) * 0.001f, 0.0f, 0.0f));
        //to_send.modelViewProj = glm::rotate(to_send.modelViewProj, glm::radians(-75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //to_send.modelViewProj = glm::perspective(35.0f, 1.0f, 0.1f, 100.0f) * to_send.modelViewProj;
        i++;
        tshader->set_matrix(to_send);
    }

    vMemoryManager::Data vTerrainModel::processData()
    {
       
        uint32_t total_size = this->getNumberVertices();
        vShader_terrain::terrain_Vertex* vertices = new vShader_terrain::terrain_Vertex[total_size];

        for(size_t i = 0; i < this->heightChunks.size(); i++)
        {
            uint32_t counter = 0;
            this->quad_num = 6;

            HeightMapChunk chunk = this->heightChunks[i];

            for(uint32_t W = 0; W < this->current_res_W; W++)
            {
                for(uint32_t H = 0; H < this->current_res_H; H++)
                {
                    glm::vec3 center = { 
                        ((int)(W + chunk.width_coord)) * this->chunk_size_W, 
                        ((int)(H + chunk.height_coord)) * this->chunk_size_H, 
                        0.5f};
                    
                    center = openGLToVulkanVector(center);

                    glm::vec3 topLeft = glm::vec3(-this->chunk_size_W / 2.0f, this->chunk_size_H / 2.0f, 0.0f);
                    glm::vec3 topRight = glm::vec3(this->chunk_size_W / 2.0f, this->chunk_size_H / 2.0f, 0.0f);
                    glm::vec3 bottomleft = glm::vec3(-this->chunk_size_W / 2.0f, -this->chunk_size_H / 2.0f, 0.0f);
                    glm::vec3 bottomRight = glm::vec3(this->chunk_size_W / 2.0f, -this->chunk_size_H / 2.0f, 0.0f);

                    
                    topLeft = openGLToVulkanVector(topLeft);
                    topRight = openGLToVulkanVector(topRight);
                    bottomleft = openGLToVulkanVector(bottomleft);
                    bottomRight = openGLToVulkanVector(bottomRight);

                    // generate quad
                    vertices[W * this->current_res_H * quad_num + quad_num*H] = {center + bottomleft};
                    vertices[W * this->current_res_H * quad_num + quad_num*H + 1] = {center + bottomRight};
                    vertices[W * this->current_res_H * quad_num + quad_num*H + 2] = {center + topRight};

                    
                    vertices[W * this->current_res_H * quad_num + quad_num*H + 3] = {center + topRight};
                    vertices[W * this->current_res_H * quad_num + quad_num*H + 4] = {center + topLeft};
                    vertices[W * this->current_res_H * quad_num + quad_num*H + 5] = {center + bottomleft};
                    //vertices[W * this->current_res_H * quad_num + quad_num*H + 3] = {topLeft};

                }
            }
        }

        return {vertices, total_size * sizeof(vShader_terrain::terrain_Vertex)};
    }
}