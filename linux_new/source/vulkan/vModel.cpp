#include "../../include/vulkan/vModel.h"
#include "../../include/vulkan/vPipeline.h"
#include "../../include/math/algorithms.h"
#include "../../include/io/PseudoJson.h"
#include "../../include/vulkan/vDynamicShader.h"


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
    vMemoryManager::Data vTerrainModel::processData()
    {
        vMemoryManager::Data ret;
        //std::vector<vShader_terrain::terrain_Vertex> vertices;
        //vShader_terrain* tshader = static_cast<vShader_terrain*>(this->shader);
        uint32_t total_size = this->current_res_W * this->current_res_H * 4;
        vShader_terrain::terrain_Vertex* vertices = new vShader_terrain::terrain_Vertex[total_size];

        for(uint32_t W = 0; W < this->current_res_W; W++)
        {
            for(uint32_t H = 0; H < this->current_res_H; H++)
            {
                vShader_terrain::terrain_Vertex vertexToAdd;

                glm::vec3 center = { W * this->chunk_size_W, H * this->chunk_size_H, 0.0f};

                glm::vec3 topLeft = center + glm::vec3(-this->chunk_size_W / 2.0f, this->chunk_size_H / 2.0f, 0.0f);
                glm::vec3 topRight = center + glm::vec3(this->chunk_size_W / 2.0f, this->chunk_size_H / 2.0f, 0.0f);
                glm::vec3 bottomleft = center + glm::vec3(-this->chunk_size_W / 2.0f, -this->chunk_size_H / 2.0f, 0.0f);
                glm::vec3 bottomRight = center + glm::vec3(this->chunk_size_W / 2.0f, -this->chunk_size_H / 2.0f, 0.0f);

                // generate quad
                vertices[W * this->current_res_H * 4 + 4*H] = {bottomleft};
                vertices[W * this->current_res_H * 4 + 4*H + 1] = {bottomRight};
                vertices[W * this->current_res_H * 4 + 4*H + 2] = {topRight};
                vertices[W * this->current_res_H * 4 + 4*H + 3] = {topLeft};
            }
        }

        return {vertices, total_size * sizeof(vShader_terrain::terrain_Vertex)};
    }
}