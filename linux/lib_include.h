#pragma once

// Custom classes
#include "io/logger.h"

// 3D graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>

// // standart default libraries
// #include <optional>
// #include <exception>
// #include <cstdlib>
// #include <iostream>
// #include <string>
// #include <vector>
// #include <array>
// #include <algorithm>
// #include <iterator>
// #include <set>
// #include <cstring>
// #include <limits>
// #include <cstdint>
// #include <stdlib.h>
// #include <fstream>
// #include <sstream>
namespace PL
{
// Constants
static const std::string LAYER_KHR_VALIDATION = "VK_LAYER_KHRONOS_validation";
static const std::string LAYER_KHR_SWAPCHAIN = "VK_KHR_swapchain";
static const std::string LAYER_EXT_DEBUG_UTILS = "VK_EXT_debug_utils";

static const std::string SHADER_DEFAULT_PATH = "shaders/";
static const std::string SHADER_SPIRV_FORMAT = ".spv";
static const std::vector<const char*> SHADER_TO_USE = 
{
    "simple",
    "normal"
};

static const int MAX_FRAMES_IN_FLIGHT = 2;

// Varibles = Objects
static const LoggerType LOGGER_TYPE = LOGGER_TYPE_FILE;
static const LoggerVerbosity LOGGER_VERBOSITY = LOGGER_VERB_ALL;

static const std::string DEVICE_MAIN = "__device_main";
static const std::string DEVICE_SUPPORT = "__device_support";

static const std::string WINDOW_MAIN = "__window_main";

}
/*
STANDING HERE
I REALIZE
YOU WERE JUST LIKE ME
TRYING TO MAKE A HISTORY
BUT WHOS TO JUDGE
A RIGHT FROM WRONG
WHEN OUR GUARD IS DOWN
I THINK WE BOTH AGREE
THAT VIOLENCEEEEEE BRING VIOLENCEEEEEEE
BUT IN THE END
IT HAS TO BE THIS WAYYYYYYYYYYYYY


            O   |                   > nanomachines son!
          - | - |                O
            |                |o__|__o|
           | |                   |
                                | |

        KOD
        napisal, 4o dalshe

        УВОЛЬТЕ ПОЖАЛУЙСТА
        Я ЗАЕБАЛСЯ
*/