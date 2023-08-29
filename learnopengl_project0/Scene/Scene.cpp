#include "Scene.h"

void Scene::generatePlane()
{
    plane = GeneratedModel(glm::mat4(1.0));
}
Scene::Scene()
{
    generatePlane();//GeneratedModel自带model矩阵
#pragma region load lionModel
    std::string LionmodelFileDirectory = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Lion/Geo_Lion.fbx";
    glm::mat4 model4lion = glm::mat4(1.0f);

    /*model4lion = glm::scale(model4lion, glm::vec3(0.002f, 0.002f, 0.002f));*/
    //model4lion = glm::translate(model4lion, glm::vec3(0.0f, 0.0f, -2.0f));
    model4lion = glm::rotate(model4lion, 180.0f, glm::vec3(0.0, 1.0, 0.0));
    model4lion = glm::scale(model4lion, glm::vec3(0.002f, 0.002f, 0.002f));
    model4lion = glm::translate(model4lion, glm::vec3(0.0f, -104.0f, 0.0f)); // translate it down so it's at the center of the scene
    //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    models.push_back(Model(LionmodelFileDirectory, MODEL_FORMAT_FBX));
    model4models.push_back(model4lion);
#pragma endregion
    light = Light();
    drawDepthShader = nullptr;
    drawModelShader = nullptr;
    drawPlaneShader = nullptr;
#pragma region catchMeshPtrs
    /*for(int i = 0; i < models.size(); i++)
    {
        for(int j = 0; j < models[i].meshes.size(); j++)
        {
            auto mesh = models[i].meshes[j];
            mesh.model = model4models[i];
            
            scene_meshes.push_back(&mesh);
        }
    }*/
#pragma endregion
}
void Scene::SetSceneShader()
{
    std::string vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Shader/drawDepth_vs.glsl";
    std::string fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Shader/drawDepth_fs.glsl";
    this->drawDepthShader = new Shader(vsPath.c_str(), fsPath.c_str());
    /*this->drawModelShader = drawModelShader;
    this->drawPlaneShader = drawPlaneShader;*/
}
void Scene::SetDepthMap()
{
    glGenFramebuffers(1, &depthFBO);
    
    glGenTextures(1, &depthMapTextureID);
    glBindTexture(GL_TEXTURE_2D, depthMapTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextureID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::DrawScene()
{
    /*for(auto mesh : scene_meshes)
    {
        mesh->Draw(*drawDepthShader, true);
    }*/
    DrawPlaneDepth();
    DrawModelDepth();

    glViewport(0, 0, 1920, 1080);
    DrawPlane(Tool::camera);
    DrawModel(Tool::camera);
}

#pragma region 未测试
void Scene::DrawSkyBox()
{
#pragma region 准备天空盒纹理
    if(skyboxTextureID == 0)
    {
        
    }
#pragma endregion

#pragma region 绘制天空盒
#pragma endregion
    
}

#pragma endregion

#pragma region 通过_正确绘制深度纹理
void Scene::DrawModel(Camera& camera)
{
    glViewport(0, 0, 1920, 1080);
    
    if(drawModelShader == nullptr)
    {
        string vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Shader/direct_light_pbr_vs.glsl";
        string fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Shader/direct_light_pbr_fs.glsl";
        drawModelShader = new Shader(vsPath.c_str(),fsPath.c_str());
    }
    drawModelShader->use();
    drawModelShader->setVec3("lightPos", light.Position);
    drawModelShader->setVec3("cameraPos", camera.Position);

    drawModelShader->setMat4("view", camera.GetViewMatrix());
    glm::mat4 projection = camera.GetCameraPerspective();
    drawModelShader->setMat4("projection", projection);

    for(int i = 0; i < models.size(); i++)
    {
        drawModelShader->setMat4("model", model4models[i]);
        
        for(auto mesh : models[i].meshes)
        {
            mesh.Draw(*drawModelShader, true);
        }
    }
    /*
    * uniform vec3 lightPos;

uniform vec3 cameraPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_maskmap;
uniform sampler2D texture_AO1;

    uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
     */
}
void Scene::DrawPlaneDepth()
{
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    drawDepthShader->use();
    drawDepthShader->setMat4("model", plane.m_model);
    drawDepthShader->setMat4("lightVP", light.LightVP);
    glBindVertexArray(plane.m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene::DrawModelDepth()
{
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    
    drawDepthShader->use();
    drawDepthShader->setMat4("lightVP", light.LightVP);
    
    for(int i = 0; i < models.size(); i++)
    {
        drawDepthShader->setMat4("model", model4models[i]);
        for(auto mesh : models[i].meshes)
        {
            //逐模型逐网格遍历
            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene::DrawPlane(Camera& camera)
{
    if(drawPlaneShader == nullptr)
    {
        string vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Shader/plane_vs.glsl";
        string fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Shader/plane_fs.glsl";
        drawPlaneShader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    if(plane.m_textureID == -1)
    {
        plane.loadGenModelTexture();
    }
    glBindVertexArray(plane.m_VAO);
    drawPlaneShader->use();
    
    drawPlaneShader->setTexture("planeTexture", plane.m_textureID, 1);
    drawPlaneShader->setTexture("shadowMap", depthMapTextureID, 2);
    
    drawPlaneShader->setMat4("model", plane.m_model);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 perspective = camera.GetCameraPerspective();
    drawPlaneShader->setMat4("view", view);
    drawPlaneShader->setMat4("prospective", perspective);
    drawPlaneShader->setMat4("lightVP", light.LightVP);
    drawPlaneShader->setVec3("lightPos", light.Position);
    drawPlaneShader->setVec3("viewPos", camera.Position);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
#pragma endregion

#pragma region 垃圾桶
/*void Scene::RenderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}*/
#pragma endregion