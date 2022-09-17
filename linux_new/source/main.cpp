#include <stdexcept>
#include "../include/general/DependFactory.h"
#include "../include/vulkan/vApplication.h"



int main() {

    

    try {
        PL::vApplication* app = static_cast<PL::vApplication*>(
            PL::DependFactory::I()->createOrGetInstance(
                PL::vApplication::_DEP_ID
            )[0]
        );
        app->MainLoop();
        
    } catch (const std::exception& e) {

        //std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
