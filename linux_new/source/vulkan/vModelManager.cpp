#include "../../include/vulkan/vModelManager.h"

#include "../../include/io/PseudoJson.h"
#include "../../include/vulkan/vModel.h"
#include "../../include/vulkan/vPipeline.h"

namespace PL
{
    

    vModelManager::~vModelManager()
    {

    }

    void vModelManager::readAllModelsFromJSON(std::string jsonFileName)
    {
        PJSON pjson = PseudoJson::readFile(jsonFileName);

        if(!pjson.hasKey("models")) 
            throw std::runtime_error("Wrong JSON format for model definition!");

        auto& models = pjson["models"];

        for( auto &j : models.ArrayRange())
        {
            if(!j.hasKey("type") || !j.hasKey("src") || !j.hasKey("shader") || !j.hasKey("input"))
            {
                throw std::runtime_error("Wrong JSON format for model definition!" + j.ToString());
            }
            
            this->readShaderInfo(j);
            std::string shaderName = j["shader"].ToString();
            
            auto pipeline = this->createdPipelines[shaderName];
            auto model = vModel::createModelFactory(j, this->memoryManager, pipeline->GetShader());
            pipeline->AddModel(model);
            break;
        }
    }

    void vModelManager::readShaderInfo(PJSON settings)
    {
        std::string shaderName = settings["shader"].ToString();
        std::string input_type = settings["input"].ToString();

        if(input_type == "quad")
        {
            this->pipeConfig->SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST, false);
        }
        else if(input_type == "tri")
        {
            this->pipeConfig->SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        }

        if(this->createdPipelines.find(shaderName) == this->createdPipelines.end())
        {
            this->pipeConfig->SetShaderName(shaderName);
            this->createdPipelines[shaderName] = this->pipeConfig->GeneratePipeline();
        }

    }

    
    void vModelManager::RecordAllPipelines(VkCommandBuffer& currentBuffer)
    {
        for(auto const& [key, value] : this->createdPipelines)
        {
            value->RenderAll(currentBuffer);
        }
    }

    void vModelManager::Initialize()
    {
        this->readAllModelsFromJSON("models/models.json");
    }

}