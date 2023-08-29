#pragma once
#include <vector>

#include "GeneratedModel.h"
#include "../Light/Light.h"
#include "../Model/Model.h"

class Scene
{
public:
    //std::vector<Light> lights; 暂时不考虑多光源，即便考虑，目前也是前向渲染的流程，遍历lightsX遍历models
    Light light;
    std::vector<Model> models;
    std::vector<glm::mat4> model4models; 
    std::vector<Mesh*> scene_meshes;
    glm::vec3 CameraInitialPosition;
    GeneratedModel plane;

    GLuint depthFBO = 0;
    GLuint depthMapTextureID = 0;

    GLuint skyboxTextureID = 0;

    Shader* drawDepthShader;
    Shader* drawPlaneShader;
    Shader* drawModelShader;
    
    Scene();
    void generatePlane();
    void DrawScene();
    void SetSceneShader();
    void SetDepthMap();
    void DrawPlane(Camera&);
    void DrawPlaneDepth();
    void DrawModel(Camera&);
    void DrawModelDepth();
    void DrawSkyBox();
    void RenderQuad();
};
