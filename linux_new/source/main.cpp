#include <stdexcept>
#include "../include/general/DependFactory.h"
#include "../include/vulkan/vApplication.h"
#include "../include/vulkan/vModel.h"
#include "../include/vulkan/vModelManager.h"
#include "../include/math/algorithms.h"

int main() {

    // auto model = static_cast<PL::vModelManager*>(
    //         PL::DependFactory::I()->createOrGetInstance(
    //             PL::vModelManager::_DEP_ID
    //         )[0]
    //     );
    // model->readAllModelsFromJSON("models/models.json");

    try {
        
        PL::vApplication* app = static_cast<PL::vApplication*>(
            PL::DependFactory::I()->createOrGetInstance(
                PL::vApplication::_DEP_ID
            )[0]
        );
        app->MainLoop();
        
    } catch (const std::exception& e) {

        PL::vApplication::GLOBAL_LOGGER << PL::LOG_MSG_ERROR << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
