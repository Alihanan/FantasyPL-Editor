#include "../../include/vulkan/vModelManager.h"


namespace PL
{
    const std::string vModelManager::_DEP_ID = IDependent::type(vModelManager());//typeid(*(new vPipeline())).name();

    vModelManager::~vModelManager()
    {

    }


}