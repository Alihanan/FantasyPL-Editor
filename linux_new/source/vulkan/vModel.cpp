#include "../../include/vulkan/vModel.h"

namespace PL
{
    const std::string vModel::_DEP_ID = IDependent::type(*(new vModel()));//typeid(*(new vPipeline())).name();

    vModel::~vModel()
    {

    }
    
    void vModel::Initialize()
    {
        
    }
}