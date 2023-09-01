#include "Scene.h"
#include "../m_Tool/Tool.h"

extern std::string m_root_dir;

void Scene::generatePlane()
{
    plane = GeneratedModel(glm::mat4(1.0));
}
Scene::Scene()
{
    generatePlane();//GeneratedModel自带model矩阵
#pragma region load lionModel
    std::string LionmodelFileDirectory =  m_root_dir + "Lion/Geo_Lion.fbx";
    glm::mat4 model4lion = glm::mat4(1.0f);
    
    //model4lion = glm::translate(model4lion, glm::vec3(0.0f, 0.0f, -2.0f));
    model4lion = glm::rotate(model4lion, 180.0f, glm::vec3(0.0, 1.0, 0.0));
    model4lion = glm::scale(model4lion, glm::vec3(0.02f, 0.02f, 0.02f));
    model4lion = glm::translate(model4lion, glm::vec3(0.0f, -100.0f, 0.0f)); // translate it down so it's at the center of the scene
    //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    models.push_back(Model(LionmodelFileDirectory, MODEL_FORMAT_FBX));
    model4models.push_back(model4lion);
#pragma endregion
    light = Light();
    drawDepthShader = nullptr;
    drawModelShader = nullptr;
    drawPlaneShader = nullptr;
    drawSkyBoxShader = nullptr;
    capture_hdr_shader = nullptr;
    reimanns_sum_shader = nullptr;
    
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
    std::string vsPath = m_root_dir + "Shader/drawDepth_vs.glsl";
    std::string fsPath = m_root_dir + "Shader/drawDepth_fs.glsl";
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthMapTextureID, 0);
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
    DrawEnv();
    //DrawSkyBox(Tool::camera);
    
    DrawPlaneDepth();
    DrawModelDepth();
    glViewport(0, 0, 1920, 1080);
    DrawPlane(Tool::camera);
    DrawModel(Tool::camera);
}

void Scene::PrepareEnvironmentCubemap()
{
#pragma region 读入hdr纹理
    if(hdrEnvTextureID == 0)
    {
        std::string path = "textures/hdr/newport_loft.hdr";
        hdrEnvTextureID =
            Model::TextureFromFile_HDREnvMap(path.c_str(), m_root_dir);
    }
#pragma endregion
#pragma region 配置FBO&RBO
    if(captureFBO == 0)
    {
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);
    }
    glBindBuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    glRenderbufferStorage(GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
#pragma endregion
#pragma region 初始化envCubemapID
    if(envCubemapID == 0)
    {
        glGenTextures(1, &envCubemapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapID);
    }
    for(unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_RGB16F, 512, 512, 0, GL_RGB,
            GL_FLOAT, nullptr);
    }//初始化了cube纹理，目前是6面空像素
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#pragma endregion
#pragma region 准备天空盒顶点
    if(skyboxVAO == 0)
    {
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);

        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

#pragma region 顶点数据
        float skyboxVertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
#pragma endregion
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),
            &skyboxVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
#pragma endregion
#pragma region 准备HDR写入到cubemap的着色器
    if(capture_hdr_shader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/hdr_capture_skybox_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/hdr_capture_skybox_fs.glsl";
        capture_hdr_shader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
#pragma endregion
#pragma region 向cubemap执行绘制
    // convert HDR equirectangular environment map to cubemap equivalent
    capture_hdr_shader->use();
    //captureProjection = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    captureProjection = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    captureViews.clear();
    captureViews = 
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
     }; //右 左 上 下 前 后 六个方向
    capture_hdr_shader->setInt("equirectangularMap", 0);
    capture_hdr_shader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrEnvTextureID);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        capture_hdr_shader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //renderCube();
        glDrawArrays(GL_TRIANGLES, 0, 36); //向cubemap绘制
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);
#pragma endregion
}
void Scene::ReimannsSumConvolution()
{
#pragma region 配置卷积shader
    if(reimanns_sum_shader == nullptr)
    {
        string vsPath = m_root_dir + "Shader/convolution_radiance_vs.glsl";
        string fsPath = m_root_dir + "Shader/convolution_radiance_fs.glsl";
        reimanns_sum_shader = new Shader(vsPath.c_str(),fsPath.c_str());
    }
#pragma endregion
#pragma region 配置convolutionIrradianceCubeMap
    if(convolutionIrradianceCubeMapID == 0)
    {
        glGenTextures(1, &convolutionIrradianceCubeMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, convolutionIrradianceCubeMapID);
        for(int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                64, 64, 0, GL_RGB, GL_FLOAT, nullptr);
            //512x512的原图大小，卷积之后只剩下32x32，这是因为实际上不再需要那么高的分辨率。
            //卷积会使图像高频信号缺失，变得非常模糊
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    //irradiance的六个面现在都是空数据
    #pragma endregion
#pragma region 缩放framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 64, 64);
#pragma endregion

#pragma region 绘制辐照图
    reimanns_sum_shader->use();
    reimanns_sum_shader->setCubeTexture("environmentCubemap", static_cast<int>(envCubemapID), 0);
    reimanns_sum_shader->setMat4("projection", captureProjection);

    glViewport(0, 0, 64, 64);
    glBindVertexArray(skyboxVAO);//prepare内 skyboxVAO已经写好
    for(int i = 0; i < 6; i++)
    {
        reimanns_sum_shader->setMat4("view", captureViews[i]);
        //设置convolutionIrradianceCubeMap作为当前FrameBuffer的附件
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, convolutionIrradianceCubeMapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion
}

void Scene::DrawEnv()
{
#pragma region 直接从HDR抽取并绘制天空盒
    /*glBindVertexArray(skyboxVAO);
    capture_hdr_shader->use();
    glm::mat4 view = glm::mat4(glm::mat3(Tool::camera.GetViewMatrix()));
    glm::mat4 perspective = Tool::camera.GetCameraPerspective();
    capture_hdr_shader->setMat4("view", view);
    capture_hdr_shader->setMat4("projection", perspective);
    capture_hdr_shader->setTexture("equirectangularMap", hdrEnvTextureID, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);*/
#pragma endregion

    //测试辐照度贴图
    glViewport(0, 0, 1920, 1080);
#pragma region 把prepare的cubemap当成天空盒渲染
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(skyboxVAO);
    if(drawSkyBoxShader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/skybox_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/skybox_fs.glsl";
        drawSkyBoxShader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    #pragma region 绘制天空盒
    drawSkyBoxShader->use();
    glm::mat4 view = glm::mat4(glm::mat3(Tool::camera.GetViewMatrix()));//取上三矩阵，消除平移
    glm::mat4 perspective = Tool::camera.GetCameraPerspective();
    drawSkyBoxShader->setMat4("view", view);
    drawSkyBoxShader->setMat4("projection", perspective);
    drawSkyBoxShader->setCubeTexture("skybox", static_cast<int>(convolutionIrradianceCubeMapID), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    #pragma endregion
#pragma endregion
    
}
void Scene::DrawModel(Camera& camera)
{
    glViewport(0, 0, 1920, 1080);
    
    if(drawModelShader == nullptr)
    {
        string vsPath = m_root_dir + "Shader/direct_light_pbr_vs.glsl";
        string fsPath = m_root_dir + "Shader/direct_light_pbr_fs.glsl";
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
        string vsPath = m_root_dir + "Shader/plane_vs.glsl";
        string fsPath = m_root_dir + "Shader/plane_fs.glsl";
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
void Scene::DrawSkyBox(Camera& camera)
{
    glDepthFunc(GL_LEQUAL);
#pragma region 准备天空盒纹理
    if(skyboxTextureID == 0)
    {
        const std::string skybox_dir = m_root_dir + "textures/skybox";
        skyboxTextureID = Model::TextureFromFile_CubeMap(skybox_dir);
    }
#pragma endregion
#pragma region 准备天空盒顶点
    if(skyboxVAO == 0)
    {
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);

        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

#pragma region 顶点数据
        float skyboxVertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
#pragma endregion
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),
            &skyboxVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
#pragma endregion
#pragma region 准备天空盒绘制着色器
    if(drawSkyBoxShader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/skybox_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/skybox_fs.glsl";
        drawSkyBoxShader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
#pragma endregion
#pragma region 绘制天空盒
    glBindVertexArray(skyboxVAO);
    drawSkyBoxShader->use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 perspective = camera.GetCameraPerspective();
    drawSkyBoxShader->setMat4("view", view);
    drawSkyBoxShader->setMat4("projection", perspective);
    drawSkyBoxShader->setTexture("skybox", skyboxTextureID, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
#pragma endregion

    glDepthFunc(GL_LESS);
}

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


#pragma region 配置辐照度图的FBO&RBO
//一开始，我企图把captureFBO再次复用，但后来引发了问题，captureFBO的尺寸是512x512，而我们现在需要一个64x64的FBO
/*if(irradianceFBO == 0)
{
    glGenFramebuffers(1, &irradianceFBO);
    glGenRenderbuffers(1, &irradianceRBO);
}
glBindBuffer(GL_FRAMEBUFFER, irradianceFBO);
glBindRenderbuffer(GL_RENDERBUFFER, irradianceRBO);

glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 64, 64);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, irradianceRBO);*/
#pragma endregion
#pragma endregion