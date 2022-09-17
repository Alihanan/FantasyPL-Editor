#include "vulkan/vulkanapp.h"






int main2() {
    try {
        PL::VulkanApp* vapp = new PL::VulkanApp();
        vapp->initializeVulkan("Fantasy Editor PL");
        vapp->createWindow(640, 320);
        
        //vapp->createModels({});
        vapp->mainLoop();
        delete vapp;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
