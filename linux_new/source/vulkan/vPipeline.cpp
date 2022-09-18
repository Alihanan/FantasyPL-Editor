#include "../../include/vulkan/vPipeline.h"

namespace PL
{
    const std::string vPipeline::_DEP_ID = IDependent::type(*(new vPipeline()));//typeid(*(new vPipeline())).name();

    vPipeline::~vPipeline()
    {

    }
    
    void vPipeline::Initialize()
    {
        
    }
}