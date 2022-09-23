#include "../../include/vulkan/vModelManager.h"

#include "../../include/io/PseudoJson.h"

namespace PL
{
    const std::string vModelManager::_DEP_ID = IDependent::type(vModelManager());//typeid(*(new vPipeline())).name();

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
            if(!j.hasKey("type") || !j.hasKey("src"))
            {
                throw std::runtime_error("Wrong JSON format for model definition!");
            }

            std::string type = j["type"].ToString();
            std::string srcFile = j["src"].ToString();
            
            std::cout << type << ", " << srcFile << std::endl;
            return;
        }
    }

}