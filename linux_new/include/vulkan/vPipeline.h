#pragma once

#include "../general/IUncopiable.h"
#include "../general/IDependent.h"

#include "vDevice.h"
#include "vPipeConfig.h"

namespace PL
{
    class vPipeline 
    {
    public:
        // Others
        vPipeline(VkPipeline* pipe);
        ~vPipeline();
        VkPipeline* graphicsPipeline;        
    protected:
        
    };


}