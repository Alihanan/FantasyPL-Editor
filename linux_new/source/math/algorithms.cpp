#include "../../include/math/algorithms.h"

namespace PL
{
    std::vector<std::string> split_string(std::string source, std::string splitters)
    {
        std::string delims = splitters;
        if(splitters.size() == 0)
            delims = " ";

        std::stringstream stringStream(source);
        std::string line;

        std::vector<std::string> wordVector;


        char first_delim = delims[0];
        delims = delims.erase(0);

        while(std::getline(stringStream, line, first_delim)) 
        {
            std::size_t prev = 0, pos;
            while ((pos = line.find_first_of(delims, prev)) != std::string::npos)
            {
                if (pos > prev)
                    wordVector.push_back(line.substr(prev, pos-prev));
                prev = pos+1;
            }
            if (prev < line.length())
                wordVector.push_back(line.substr(prev, std::string::npos));
        }

        return wordVector;
    }

    glm::mat4 glmEulerRotation(float xRotation_roll_angle, float yRotation_pitch_angle, float zRotation_yaw_angle)
    {
        float xRotation_roll = glm::radians(xRotation_roll_angle);
        float yRotation_pitch = glm::radians(yRotation_pitch_angle);
        float zRotation_yaw = glm::radians(zRotation_yaw_angle);


        float cosX = glm::cos(xRotation_roll);
        float sinX = glm::sin(xRotation_roll);      
        glm::mat3 rotX = glm::mat3({
            {cosX, sinX, 0.0f}, 
            {-sinX, cosX, 0.0f}, 
            {0.0f, 0.0f, 1.0f}
            });

        float cosY = glm::cos(yRotation_pitch);
        float sinY = glm::sin(yRotation_pitch);
        glm::mat3 rotY = glm::mat3({
            {cosY, 0.0f, -sinY}, 
            {0.0f, 1.0f, 0.0f},
            {sinY, 0.0f, cosY}, 
            });

        float cosZ = glm::cos(zRotation_yaw);
        float sinZ = glm::sin(zRotation_yaw);
        glm::mat3 rotZ = glm::mat3({
            {1.0f, 0.0f, 0.0f}, 
            {0.0f, cosZ, sinZ},
            {0.0f, -sinZ, cosZ}, 
            });

        glm::mat3 rot = rotX * rotY * rotZ;
        
        return glm::mat4(rot);
    }

    glm::vec4 openGLToVulkanVector(glm::vec4 vec)
    {
        vec.g = -vec.g;
        return vec;
    }
    glm::vec3 openGLToVulkanVector(glm::vec3 vec)
    {
        glm::vec4 ret = openGLToVulkanVector(glm::vec4(vec, 1.0f));
        return {ret.r, ret.g, ret.b};
    }
}