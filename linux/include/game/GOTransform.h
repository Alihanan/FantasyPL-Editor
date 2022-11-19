#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

namespace PL
{
    class GOTransform
    {
    public:
        GOTransform();

        glm::mat4 M()
        {
            glm::mat4 ret = glm::mat4(1.0);
            ret = glm::translate(ret, {x, y, z});
            //ret = glm::rotateX(ret, rotX);
            //ret = glm::rotateY(ret, rotY);
            //ret = glm::rotateZ(ret, rotZ);

            return ret;
        }

        void translate(float x, float y, float z)
        {
            this->x += x;
            this->y += y;
            this->z += z;
        }

    protected:
        float x {0.0f};
        float y {0.0f};
        float z {0.0f};
        float rotX {0.0f};
        float rotY {0.0f};
        float rotZ {0.0f};

    };
}