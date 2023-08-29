#ifndef TOOL_H
#define TOOL_H

#include "../Model/Model.h"
#include "../Camera/camera.h"
//#include <glad/glad.h>
#include <GLFW/glfw3.h>

#pragma region ShaderTypeMacro
#define DRAW_1_TRIANGLE 1001
#define DRAW_1_TRIANGLE_WITH_EBO 1002
#define TRIANGLE_TIME_SHIFT_COLOR 1003
#define TRIANGLE_COLOR_INTERPOLATION 1004
#define RECTANGLE_TEXTURE 1005
#define RECTANGLE_2_TEXTURES 1006
#define DRAW_CUBE_NO_ZBUFFER 1007
#define DRAW_CUBE_ZBUFFER 1008
#define BASIC_ILLUMINATION 1009
#define PHONG_SHADING 1010
#define SHADOW_MAP 1011
#define PBR_DIRECT_LIGHT_SHADER 1020
#define BASIC_TEXTURING_SHADER 1050
#pragma endregion

class Model;
class Tool
{
     
public:
#pragma region InputRelatedMembers
    static float deltaTime;
    static float lastFrame;
    static float currentFrame;
    static int screenWidth;
    static int screenHeight;
    static float lastX;
    static float lastY;
    static bool firstMouse;

    static Camera camera;
#pragma endregion

#pragma region AssetRelatedMembers
    static int model_format;
    static std::string modelFileDirectory;
    static std::string shaderFileDirectory;
    static unsigned int planeTextureID;
#pragma endregion

#pragma region DepthRelatedMembers
    static GLuint depthMapFBO;
    static GLuint depthMapTextureID;
    static Shader* drawDepthShader;
    static glm::vec3 lightPosition;
#pragma endregion 
    static GLFWwindow* window;
    static glm::vec4 bgColor;
    static unsigned int planeVAO;
    
    static Model* currentUsedModel;
    static Shader* currentUsedShader;
    static Shader* planeShader;
    static glm::mat4 model4plane;
    static glm::mat4 model4model;
    static glm::mat4 view;
    static glm::mat4 prospective;
    static glm::mat4 lightVP;
    
    void static InitWindow(int screenWidth, int screenHeight);
    void static RenderLoop();
    void static Terminate();
    void static processInput();
    void static LoadModel(int);
    void static clearColor();
    void static PickModelShader(int);
    void static MakeShader(const char*, const char*);
    void static GenerateMVP();
    void static SwapFrontAndBackFrame();
    unsigned int static loadTexture(char const* path);
    void static DrawPlane();
    void static DrawPlaneDepth();
    void static GenerateDepthMapNShader();
private:
    static void Tick();
    static void TickLight(glm::vec3 &light);
};

#endif