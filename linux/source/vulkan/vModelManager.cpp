#include "../../include/vulkan/vModelManager.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcpp"
#include "../../include/io/PseudoJson.h"
#pragma GCC diagnostic pop


#include "../../include/vulkan/vModel.h"
#include "../../include/vulkan/vPipeline.h"


namespace PL
{
    

    vModelManager::~vModelManager()
    {

    }

    void vModelManager::setInputVertType(InputVertType type)
    {
        switch(type)
        {
            case TRIANGLE_VERT_IN:
            {
                this->pipeConfig->SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
                break;
            }
            case QUAD_VERT_IN:
            {
                this->pipeConfig->SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST, false);
                break;
            }
            default:
            {
                throw std::runtime_error("UNKNOWN INPUT TYPE PASSED AS PARAM!");
                break;
            }
        }
    }

    void vModelManager::setShader(std::string shaderName)
    {
        if(this->createdPipelines.find(shaderName) == this->createdPipelines.end())
        {
            this->pipeConfig->SetShaderName(shaderName);
            this->createdPipelines[shaderName] = this->pipeConfig->GeneratePipeline();
        }
    }

    void vModelManager::ReplaceRenderModels(std::vector<GameModel*>* models)
    {
        if(models == nullptr) return;

        std::map<vPipeline*, std::vector<std::pair<vModel*, GameModel*>>*> toSend;

        for(GameModel* gm : *models)
        {   
            InputVertType* input_type = static_cast<InputVertType*>(gm->Param(PARAM_INPUT_VERT_TYPE));
            this->setInputVertType(*input_type);

            std::string* shaderFile = static_cast<std::string*>(gm->Param(PARAM_SHADER_FILE));
            this->setShader(*shaderFile); 

            auto pipeline = this->createdPipelines[*shaderFile];
            auto model = vModel::createModelFactory(gm->Type().id, this->memoryManager, pipeline->GetShader());
            //pipeline->AddModel(model);
            if(toSend.count(pipeline) == 0)
            {
                toSend[pipeline] = new std::vector<std::pair<vModel*, GameModel*>>();
            }

            toSend[pipeline]->push_back(std::pair<vModel*, GameModel*>(model, gm));
        }

        for(auto [key, value] : toSend)
        {
            key->SetModels(toSend[key]);
        }
    }

    /*
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
    }*/

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
        //this->readAllModelsFromJSON("models/models.json");
    }

}