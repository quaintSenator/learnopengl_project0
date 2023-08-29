#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Light
{
public:
    glm::vec3 Position;
    float light_near_plane;
    float light_far_plane;
    glm::mat4 LightVP;
    Light()
    {
        Position = glm::vec3(0.0, 0.5, 0.0);
        light_near_plane = 0.01f;
        light_far_plane = 7.5f;
        glm::mat4 lightV = glm::lookAt(Position, glm::vec3(0.0f), glm::vec3(1.0, 0.0, 0.0));//从顶上往下看
        glm::mat4 lightP = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, light_near_plane, light_far_plane);
        LightVP = lightP * lightV;
    }
};
