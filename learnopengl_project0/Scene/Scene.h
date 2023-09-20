#pragma once
#include <vector>

#include "GeneratedModel.h"
#include "../Light/Light.h"
#include "../Model/Model.h"

#define USE_FORWARD_RENDERING 0
#define USE_DEFERED_RENDERING 1

class Scene
{
public:
    //std::vector<Light> lights; 暂时不考虑多光源，即便考虑，目前也是前向渲染的流程，遍历lightsX遍历models
    Light light;
    std::vector<Light> lights;
    std::vector<Model> models;
    std::vector<glm::mat4> model4models; 
    std::vector<Mesh*> scene_meshes;
    glm::vec3 CameraInitialPosition;
    Model plane;

    //GLuint depthFBO = 0;
    std::vector<GLuint> depthFBOs;
    std::vector<GLuint> depthMapIDs;
    GLuint skyboxTextureID = 0;
    GLuint hdrEnvTextureID = 0; //装载的矩形hdr图
    GLuint envCubemapID = 0;//生成的环境立方体贴图
    GLuint skyboxVAO = 0;
    GLuint convolutionIrradianceCubeMapID = 0;//卷积得到的辐照图cubemap
    GLuint brdfLookUpTextureID = 0;
    GLuint captureFBO = 0;
    GLuint irradianceFBO = 0;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    GLuint prefilteredMapID = 0;
    
    GLuint gBuffer = 0;//Gbuffer对应的FBO
    GLuint gPositionTexture = 0;
    GLuint gNormalTexture = 0;
    GLuint gTangentTexture = 0;
    GLuint gBitangentTexture = 0;
    GLuint gDiffuseTexture = 0;
    GLuint gmaskmapTexture = 0;
    GLuint gAoTexture = 0;
    GLuint ssaoFBO = 0;
    GLuint ssaoBlurFBO = 0;
    GLuint ssaoColorTexture = 0;
    GLuint ssaoColorBlurTexture = 0;
    
    GLuint gDepthRBO = 0;
    
    Shader* drawDepthShader;
    Shader* drawPlaneShader;
    Shader* drawModelShader;
    Shader* drawSkyBoxShader;
    Shader* capture_hdr_shader;
    Shader* reimanns_sum_shader;
    Shader* prefilter_shader;
    Shader* lut_shader;
    Shader* defer_geometry_pass_shader;
    Shader* defer_lighting_pass_shader;
    Shader* multi_light_plane_shader;
    Shader* ssao_shader;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    GLuint noiseTexture = 0;
    
    int scene_render_mode = -1;
    glm::mat4 captureProjection;
    vector<glm::mat4> captureViews;
    
    Scene();
    void DrawScene();
    void SetSceneShader();
    void SetDepthMaps();
    void DrawPlane();
    void DrawPlaneDepth();
    void DrawScene2GBuffer();
    void DrawModels();
    void DrawModelDepth();
    void DrawSkyBox(Camera&);
    void DrawEnv();
    void PrepareEnvironmentCubemap();
    void ReimannsSumConvolution();
    void PreparePrefilteredEnvCubemap();
    void PrepareLUT();
    void PrepareSSAO();
    void PrepareSSAOSamples();
    void PrepareGBuffer();
    void RenderQuad();
private:
    GLuint skyboxVBO;
    GLuint captureRBO = 0;
    GLuint irradianceRBO = 0;
};
