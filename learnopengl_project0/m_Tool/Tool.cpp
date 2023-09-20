#include "Tool.h"
#include <windows.h>
#include "stb_image.h"
#include <iostream>
#include "../Scene/Scene.h"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

std::string m_root_dir;

float Tool::deltaTime = 0.0;
float Tool::lastFrame = 0.0;
float Tool::currentFrame = 0.0;
int Tool::screenWidth = 0;
int Tool::screenHeight = 0;
float Tool::lastX = 0.0;
float Tool::lastY = 0.0;
bool Tool::firstMouse = false;
Camera Tool::camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
GLFWwindow* Tool::window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
glm::vec4 Tool::bgColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
glm::mat4 Tool::model4plane = glm::mat4(1.0);
glm::mat4 Tool::model4model = glm::mat4(1.0);
glm::mat4 Tool::view = glm::mat4(1.0);
glm::mat4 Tool::prospective = glm::mat4(1.0);
glm::mat4 Tool::lightVP = glm::mat4(1.0);
glm::vec3 Tool::lightPosition = glm::vec3(1.0);

//std::string Tool::modelFileDirectory = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Lion/Geo_Lion.fbx";
std::string Tool::modelFileDirectory = m_root_dir + "Lion/Geo_Lion.fbx";
std::string Tool::shaderFileDirectory = m_root_dir + "Shader";
int Tool::model_format = MODEL_FORMAT_FBX;
unsigned int Tool::planeTextureID = 0;
unsigned int Tool::planeVAO = -1;

GLuint Tool::depthMapFBO = 0;
GLuint Tool::depthMapTextureID = 0;
Model* Tool::currentUsedModel = nullptr;
Shader* Tool::currentUsedShader = nullptr;
Shader* Tool::planeShader = nullptr;
Shader* Tool::drawDepthShader = nullptr;

void Tool::InitWindow(int Width, int Height)
{
#pragma region 输入和cameraInit
    screenWidth = Width;
    screenHeight = Height;
    
    lastX = (float)screenWidth / 2.0;
    lastY = (float)screenHeight / 2.0;
    firstMouse = true;

    deltaTime = 0.0;
    lastFrame = 0.0;
    currentFrame = 0.0;
#pragma endregion
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL V2", nullptr, nullptr);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window,double xoffset,double yoffset)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    });
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}
void Tool::RenderLoop()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene myScene = Scene();
    myScene.SetSceneShader();
    myScene.PrepareEnvironmentCubemap();
    myScene.ReimannsSumConvolution();
    myScene.SetDepthMaps();
    myScene.PreparePrefilteredEnvCubemap();
    myScene.PrepareLUT();
    myScene.PrepareGBuffer();
    myScene.PrepareSSAO();
    myScene.PrepareSSAOSamples();
    //myScene.PreparePrefilteredEnvCubemap();
    while(!glfwWindowShouldClose(window))
    {
        Tick();
        processInput();
        clearColor();
        myScene.DrawScene();
        /*drawDepthShader->use();
        drawDepthShader->setMat4("lightVP", lightVP);
        currentUsedModel->Draw2Depth(*drawDepthShader, model4model);
        DrawPlaneDepth();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        currentUsedModel->Draw(*currentUsedShader);
        DrawPlane();*/
        SwapFrontAndBackFrame();
    }
}
void Tool::Terminate()
{
    glfwTerminate();
    delete planeShader;
    delete currentUsedModel;
    delete currentUsedShader;
    delete drawDepthShader;
}
void Tool::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT_ROT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT_ROT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);  
}
void Tool::Tick()
{
    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
void Tool::TickLight(glm::vec3 &light)
{
    glm::mat4 rotationMat = glm::mat4(1.0);
    float rotateAngle = glfwGetTime();//1s 1 degrees
        
    rotationMat = glm::rotate(rotationMat, rotateAngle ,glm::vec3(0.0, 1.0, 0.0));
    glm::vec4 rotateResult = rotationMat * glm::vec4(light.x, light.y, light.z, 1.0f);
    light = glm::vec3(rotateResult.x, rotateResult.y, rotateResult.z);
}
void Tool::DrawPlaneDepth()
{
    /*float planeVertices[] = {
        // positions            // normals         // texcoords
        2.0f, -0.013f,  2.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
       -2.0f, -0.013f,  2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
       -2.0f, -0.013f, -2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
        2.0f, -0.013f,  2.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
       -2.0f, -0.013f, -2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
        2.0f, -0.013f, -2.0f,  0.0f, 1.0f, 0.0f,   5.0f,  5.0f
   };
    // plane VAO
    if(Tool::planeVAO == -1)
    {
        unsigned int planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }
    //1 pass draw depth
    drawDepthShader->use();
    drawDepthShader->setMat4("model", model4plane);
    drawDepthShader->setMat4("lightVP", lightVP);

    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);*/
}
void Tool::DrawPlane()
{
    float planeVertices[] = {
        // positions            // normals         // texcoords
        2.0f, -0.013f,  2.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
       -2.0f, -0.013f,  2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
       -2.0f, -0.013f, -2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
        2.0f, -0.013f,  2.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
       -2.0f, -0.013f, -2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
        2.0f, -0.013f, -2.0f,  0.0f, 1.0f, 0.0f,   5.0f,  5.0f
   };
    // plane VAO
    if(Tool::planeVAO == -1)
    {
        unsigned int planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }

    glBindVertexArray(planeVAO);
    
    if(planeTextureID == 0)
    {
        planeTextureID = Model::TextureFromFile(
        "wood.png",
        std::string(m_root_dir + "textures"),
        false
        );
    }
    if(planeShader == nullptr)
    {
        std::string vsPath = Tool::shaderFileDirectory + "/plane_vs.glsl";
        std::string fsPath = Tool::shaderFileDirectory + "/plane_fs.glsl";
        Tool::planeShader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    planeShader->use();
    
    /*planeShader->setMat4("model", model);
    planeShader->setMat4("view", view);
    planeShader->setMat4("prospective", prospective);*/
    glBindVertexArray(planeVAO);

    planeShader->setMat4("model", glm::mat4(1.0));
    planeShader->setMat4("view", Tool::view);
    planeShader->setMat4("prospective", Tool::prospective);

    //planeShader->setTexture("planeTexture", planeTextureID);
    /*glActiveTexture(GL_TEXTURE5);
    planeShader->setInt("planeTexture", 5);
    glBindTexture(GL_TEXTURE_2D, planeTextureID);*/
    planeShader->setTexture("planeTexture", planeTextureID, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindVertexArray(0);
}
void Tool::LoadModel(int mformat)
{
    stbi_set_flip_vertically_on_load(true);
    currentUsedModel = new Model(modelFileDirectory, mformat);
}
void Tool::clearColor()
{
    glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Tool::SwapFrontAndBackFrame()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}