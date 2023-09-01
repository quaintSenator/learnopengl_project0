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
    GLuint hdrEnvTextureID = 0; //装载的矩形hdr图
    GLuint envCubemapID = 0;//生成的环境立方体贴图
    GLuint skyboxVAO = 0;
    GLuint convolutionIrradianceCubeMapID = 0;//卷积得到的辐照图cubemap
    GLuint captureFBO = 0;
    GLuint irradianceFBO = 0;

    Shader* drawDepthShader;
    Shader* drawPlaneShader;
    Shader* drawModelShader;
    Shader* drawSkyBoxShader;
    Shader* capture_hdr_shader;
    Shader* reimanns_sum_shader;

    glm::mat4 captureProjection;
    vector<glm::mat4> captureViews;
    
    Scene();
    void generatePlane();
    void DrawScene();
    void SetSceneShader();
    void SetDepthMap();
    void DrawPlane(Camera&);
    void DrawPlaneDepth();
    void DrawModel(Camera&);
    void DrawModelDepth();
    void DrawSkyBox(Camera&);
    void DrawEnv();
    void PrepareEnvironmentCubemap();
    void ReimannsSumConvolution();
    void RenderQuad();
private:
    GLuint skyboxVBO;
    GLuint captureRBO = 0;
    GLuint irradianceRBO = 0;
};
