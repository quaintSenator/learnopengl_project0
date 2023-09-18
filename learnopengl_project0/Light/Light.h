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
    glm::vec3 color;
    Light()
    {
        Position = glm::vec3(0.0, 1, 0.0);
        light_near_plane = 0.01f;
        light_far_plane = 30.0f;
        glm::mat4 lightV = glm::lookAt(Position, glm::vec3(0.0f), glm::vec3(1.0, 0.0, 0.0));//从顶上往下看
        glm::mat4 lightP = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, light_near_plane, light_far_plane);
        LightVP = lightP * lightV;
    }
    Light(glm::vec3 lightPos)
        :Position(lightPos), light_near_plane(0.01f), light_far_plane(30.0f)
    {
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 PC = glm::normalize(center - Position);
        glm::vec3 Z = glm::cross(PC, glm::vec3(0.0, 1.0, 0.0));
        glm::vec3 up = glm::cross(PC, Z);
        //glm::mat4 lightV = glm::lookAt(Position, center, glm::vec3(1.0, 0.0, 0.0));//从顶上往下看
        glm::mat4 lightV = glm::lookAt(Position, center, up);
        glm::mat4 lightP = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, light_near_plane, light_far_plane);
        LightVP = lightP * lightV;
    }
    Light(glm::vec3 lightPos, glm::vec3 lightColor):Light(Light(lightPos))
    {
        color = lightColor;
    }
};
