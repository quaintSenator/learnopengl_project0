#include <random>
#include "Scene.h"
#include "../m_Tool/Tool.h"

extern std::string m_root_dir;

Scene::Scene()
{
    scene_render_mode = USE_DEFERED_RENDERING;
    plane = GeneratedModel(glm::mat4(1.0));//GeneratedModel自带model矩阵
#pragma region 加载lion模型
    std::string LionmodelFileDirectory =  m_root_dir + "Lion/Geo_Lion.fbx";
    glm::mat4 model4lion = glm::mat4(1.0f);
    
    //model4lion = glm::translate(model4lion, glm::vec3(0.0f, 0.0f, -2.0f));
    model4lion = glm::rotate(model4lion, 180.0f, glm::vec3(0.0, 1.0, 0.0));
    model4lion = glm::scale(model4lion, glm::vec3(0.02f, 0.02f, 0.02f));
    model4lion = glm::translate(model4lion, glm::vec3(0.0f, -94.0f, 0.0f)); // translate it down so it's at the center of the scene
    //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    models.push_back(Model(LionmodelFileDirectory, MODEL_FORMAT_FBX));
    model4models.push_back(model4lion);
#pragma endregion
    light = Light();
    lights = vector<Light>(100);
    if(scene_render_mode == USE_FORWARD_RENDERING)
    {
        lights.push_back(light);
        lights.emplace_back(glm::vec3(2.0, 2.5, 2.0));
        lights.emplace_back(glm::vec3(-2.0, 2.5, 2.0));
        lights.emplace_back(glm::vec3(2.0, 2.5, -2.0));
        lights.emplace_back(glm::vec3(-2.0, 2.5, -2.0));
    }
    if(scene_render_mode == USE_DEFERED_RENDERING)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);
    
        float delta = 1.0f;
        float y = 2.5;
        for(int i = 0; i < 10; i++)
        {
            float x = -5 + delta * i;
            for(int j = 0; j < 10; j++)
            {
                float z = -5 + delta * j;
                glm::vec3 randomColor(dis(gen), dis(gen), dis(gen));
                lights[i * 10 + j] = Light(glm::vec3(x, y, z), randomColor);
            }
        }
    }
    drawDepthShader = nullptr;
    drawModelShader = nullptr;
    drawPlaneShader = nullptr;
    drawSkyBoxShader = nullptr;
    capture_hdr_shader = nullptr;
    reimanns_sum_shader = nullptr;
    prefilter_shader = nullptr;
    lut_shader = nullptr;
    defer_geometry_pass_shader = nullptr;
    defer_lighting_pass_shader = nullptr;
    multi_light_plane_shader = nullptr;
    ssao_shader = nullptr;
}
void Scene::SetSceneShader()
{
    std::string vsPath = m_root_dir + "Shader/drawDepth_vs.glsl";
    std::string fsPath = m_root_dir + "Shader/drawDepth_fs.glsl";
    this->drawDepthShader = new Shader(vsPath.c_str(), fsPath.c_str());
    /*this->drawModelShader = drawModelShader;
    this->drawPlaneShader = drawPlaneShader;*/
}
void Scene::SetDepthMaps()
{
    if(scene_render_mode == USE_FORWARD_RENDERING)
    {
        depthMapIDs = vector<GLuint>(lights.size());
        for(int i = 0; i < lights.size(); i++)
        {
            glGenTextures(1, &depthMapIDs[i]);
            std::cout<<"Texture generated success, TextureID = "<< depthMapIDs[i] << std::endl;
            glBindTexture(GL_TEXTURE_2D, depthMapIDs[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        depthFBOs = vector<GLuint>(lights.size());
        for(int i = 0; i < lights.size(); i++)
        {
            glGenFramebuffers(1, &depthFBOs[i]);
            //glBindTexture(GL_TEXTURE_2D, depthMapIDs[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBOs[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D, depthMapIDs[i], 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            //glBindTexture(GL_TEXTURE_2D, 0);
            //glDrawBuffer(GL_NONE);
            //glReadBuffer(GL_NONE);
        }
    }
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
        std::cout<<"Env cubemap Texture generated success, TextureID = " << envCubemapID << std::endl;
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
    glViewport(0, 0, 1920, 1061);
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
        std::cout << "Texture Loaded Success,TextureID = " << convolutionIrradianceCubeMapID << std::endl;
        glBindTexture(GL_TEXTURE_CUBE_MAP, convolutionIrradianceCubeMapID);
        for(int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128);
#pragma endregion
#pragma region 绘制辐照图
    reimanns_sum_shader->use();
    reimanns_sum_shader->setCubeTexture("environmentCubemap", static_cast<int>(envCubemapID), 0);
    reimanns_sum_shader->setMat4("projection", captureProjection);

    glViewport(0, 0, 128, 128);
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
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
#pragma endregion
}
void Scene::PreparePrefilteredEnvCubemap()
{
#pragma region 初始化预滤波贴图&mipmap
    if(prefilteredMapID == 0)
    {
        glGenTextures(1, &prefilteredMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID);
        for(unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        //GenerateMipmap 仅仅生成了大小逐步缩放的几张图，而没有填入数据
        /*Level 0: 128x128
        Level 1: 64x64
        Level 2: 32x32
        Level 3: 16x16
        Level 4: 8x8
        Level 5: 4x4
        Level 6: 2x2
        Level 7: 1x1*/
    }
#pragma endregion
#pragma region 初始化prefilterShader
    if(prefilter_shader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/prefilter_hdr_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/prefilter_hdr_fs.glsl";
        prefilter_shader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    prefilter_shader->use();
    prefilter_shader->setTexture("environmentMap", static_cast<GLuint>(envCubemapID), 0);
    prefilter_shader->setMat4("projection", captureProjection);

    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapID);
    glBindVertexArray(skyboxVAO);//顶点还是skybox那批
#pragma endregion
#pragma region 逐mipmaplevel绘制[重要]
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int mipmapMaxLevel = 5;
    for(unsigned int mip = 0; mip < mipmapMaxLevel; mip++)
    {
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = mipWidth;

        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        //注，这里的RBO已经在Scene::PrepareEnvironmentCubemap 进行了绑定：
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(mipmapMaxLevel - 1);//0, 0.25, 0.5, 0.75, 1
        prefilter_shader->setFloat("roughness", roughness);
        for(unsigned int face = 0; face < 6; face++)
        {
            prefilter_shader->setMat4("view", captureViews[face]);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, prefilteredMapID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion
}
void Scene::PrepareLUT()
{
#pragma region 准备lookupTexture
    if(brdfLookUpTextureID == 0)
    {
        glGenTextures(1, &brdfLookUpTextureID);
    }
    glBindTexture(GL_TEXTURE_2D, brdfLookUpTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F,
        512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, brdfLookUpTextureID, 0);
#pragma endregion
#pragma region 准备LUTshader
    if(lut_shader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/lut_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/lut_fs.glsl";
        lut_shader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    lut_shader->use();
#pragma endregion
#pragma region 准备Quad顶点
    if(quadVAO == 0)
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
    
#pragma endregion 
    //绘制LUT
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
void Scene::PrepareSSAO()
{
    if(ssaoFBO == 0)
    {
        glGenFramebuffers(1, &ssaoFBO);
        glGenFramebuffers(1, &ssaoBlurFBO);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    if(ssaoColorTexture == 0)
    {
        glGenTextures(1, &ssaoColorTexture);
        glBindTexture(GL_TEXTURE_2D, ssaoColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            1920, 1061, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, ssaoColorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    if(ssaoColorBlurTexture == 0)
    {
        glGenTextures(1, &ssaoColorBlurTexture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
        1920, 1061, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, ssaoColorBlurTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if(ssao_shader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/ssao_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/ssao_fs.glsl";
        ssao_shader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
}
void Scene::PrepareSSAOSamples()
{
    #pragma region sample核心
    std::uniform_real_distribution<float> randomFloat(0.0, 1.0);
    std::default_random_engine gen;
    auto lerp = [](GLfloat a, GLfloat b, GLfloat f)->GLfloat
    {
        return a + f * (b - a);
    };
    for(int i = 0; i < 64; i++)
    {
        glm::vec3 sample(randomFloat(gen) * 2.0 - 1.0, randomFloat(gen) * 2.0 - 1.0, randomFloat(gen));
        sample = glm::normalize(sample);
        sample *= randomFloat(gen);
        GLfloat scale = GLfloat(i)/64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    #pragma endregion
    #pragma region 准备noise纹理
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloat(gen) * 2.0 - 1.0, randomFloat(gen) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    if(noiseTexture == 0)
    {
        glGenTextures(1, &noiseTexture);
    }
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    //noise图 大小为4x4
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    #pragma endregion
}
void Scene::PrepareGBuffer()
{
    #pragma region 准备geometrypass的着色器
    if(defer_geometry_pass_shader == nullptr)
    {
        string vsPath = m_root_dir + "Shader/geometry_pass_vs.glsl";
        string fsPath = m_root_dir + "Shader/geometry_pass_fs.glsl";
        defer_geometry_pass_shader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    if(defer_lighting_pass_shader == nullptr)
    {
        string vsPath = m_root_dir + "Shader/lighting_pass_vs.glsl";
        string fsPath = m_root_dir + "Shader/lighting_pass_fs.glsl";
        defer_lighting_pass_shader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    defer_geometry_pass_shader->use();
    #pragma endregion
    #pragma region 创建Gbuffer
    if(gBuffer == 0)
    {
        glGenFramebuffers(1, &gBuffer);
        
        glGenTextures(1, &gPositionTexture);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920,
            1061, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glGenTextures(1, &gNormalTexture);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920,
            1061, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glGenTextures(1, &gDiffuseTexture);
        glBindTexture(GL_TEXTURE_2D, gDiffuseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920,
            1061, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenTextures(1, &gAoTexture);
        glBindTexture(GL_TEXTURE_2D, gAoTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920,
            1061, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    #pragma endregion
    #pragma region 准备GBuffer
    GLuint attachments[4] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffuseTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
           GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAoTexture, 0);
    //告知OpenGL，我们即将使用哪些attachment进行绘制
    glDrawBuffers(4, attachments);
    if(gDepthRBO == 0)
    {
        glGenRenderbuffers(1, &gDepthRBO);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, gDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1061);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    #pragma endregion
}
void Scene::DrawScene()
{
    DrawEnv();
    if(scene_render_mode == USE_FORWARD_RENDERING)
    {
        DrawPlaneDepth();
        DrawModelDepth();
    }
    glViewport(0, 0, 1920, 1061);
    DrawModel();
    DrawPlane();
    DrawEnv();
}
void Scene::DrawEnv()
{
#pragma region 直接从HDR抽取并绘制天空盒
    glDepthFunc(GL_LEQUAL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(skyboxVAO);
    if(drawSkyBoxShader == nullptr)
    {
        std::string vsPath = m_root_dir + "Shader/skybox_vs.glsl";
        std::string fsPath = m_root_dir + "Shader/skybox_fs.glsl";
        drawSkyBoxShader = new Shader(vsPath.c_str(), fsPath.c_str());
    }
    drawSkyBoxShader->use();
    glm::mat4 view = glm::mat4(glm::mat3(Tool::camera.GetViewMatrix()));
    glm::mat4 perspective = Tool::camera.GetCameraPerspective();
    drawSkyBoxShader->setMat4("view", view);
    drawSkyBoxShader->setMat4("projection", perspective);
    drawSkyBoxShader->setTexture("skybox", envCubemapID, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
#pragma endregion
    //测试辐照度贴图
    glViewport(0, 0, 1920, 1061);
#pragma region 把prepare的cubemap当成天空盒渲染
    /*
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
    #pragma endregion*/
#pragma endregion
}
void Scene::DrawModel()
{
    glViewport(0, 0, 1920, 1061);
    #pragma region 前向渲染
    if(scene_render_mode == USE_FORWARD_RENDERING)
    {
        if(drawModelShader == nullptr)
        {
            string vsPath = m_root_dir + "Shader/IBL_vs.glsl";
            string fsPath = m_root_dir + "Shader/IBL_fs.glsl";
            drawModelShader = new Shader(vsPath.c_str(),fsPath.c_str());
        }
        drawModelShader->use();
        for(int i = 0; i < lights.size(); i++)
        {
            drawModelShader->setVec3("lightPos[" + std::to_string(i) + "]", lights[i].Position);
        }
        drawModelShader->setVec3("cameraPos", Tool::camera.Position);
        drawModelShader->setMat4("view", Tool::camera.GetViewMatrix());
    
        glm::mat4 projection = Tool::camera.GetCameraPerspective();
        drawModelShader->setMat4("projection", projection);
        drawModelShader->setCubeTexture("irradianceMap", convolutionIrradianceCubeMapID, 5);
        drawModelShader->setCubeTexture("prefilterMap", prefilteredMapID, 6);
        drawModelShader->setTexture("brdfLUT", brdfLookUpTextureID, 7);
        for(int i = 0; i < models.size(); i++)
        {
            drawModelShader->setMat4("model", model4models[i]);
            if(models[i].meshes.size() >= 1)
            {
                models[i].meshes[0].Draw(*drawModelShader, true);
            }
        }
    }
    #pragma endregion
    #pragma region 延迟渲染
    if(scene_render_mode == USE_DEFERED_RENDERING)
    {
        #pragma region geometry pass
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepthRBO);
        defer_geometry_pass_shader->use();
        defer_geometry_pass_shader->setMat4("perspective", Tool::camera.GetCameraPerspective());
        defer_geometry_pass_shader->setMat4("view", Tool::camera.GetViewMatrix());
        for(int i = 0; i < models.size(); i++)
        {
            defer_geometry_pass_shader->setMat4("model", model4models[i]);
            if(models[i].meshes.size() >= 1)
            {
                models[i].meshes[0].Draw(*defer_geometry_pass_shader, true, true);
            }
        }
        /*glBindVertexArray(plane.m_VAO);
        defer_geometry_pass_shader->setMat4("model", plane.m_model);
        glDrawArrays(GL_TRIANGLES, 0, 6);*/
        
        //解绑所有texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        #pragma endregion
        #pragma region 绘制SSAOtexture
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        
        #pragma endregion
        #pragma region 从GBuffer向默认FBO拷贝深度
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        //glBindFramebuffer(GL_READ_FRAMEBUFFER_BINDING, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, 1920,
            1061, 0, 0, 1920,
            1061, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        #pragma endregion
        #pragma region lighting Pass
        glDepthFunc(GL_ALWAYS);
        glDepthMask(GL_FALSE);//禁止深度写入
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        defer_lighting_pass_shader->use();
        defer_lighting_pass_shader->setTexture("G_FragPos", gPositionTexture, 6);
        defer_lighting_pass_shader->setTexture("G_Normal", gNormalTexture, 7);
        defer_lighting_pass_shader->setTexture("G_Albedo", gDiffuseTexture, 8);
        defer_lighting_pass_shader->setTexture("G_AO", gAoTexture, 9);
        defer_lighting_pass_shader->setCubeTexture("irradianceMap", convolutionIrradianceCubeMapID, 10);
        defer_lighting_pass_shader->setCubeTexture("prefilterMap", prefilteredMapID, 11);
        defer_lighting_pass_shader->setTexture("brdfLUT", brdfLookUpTextureID, 12);
        defer_lighting_pass_shader->setVec3("cameraPos", Tool::camera.Position);
        for(int i = 0; i < lights.size(); i++)
        {
            defer_lighting_pass_shader->setVec3("lightPos[" + std::to_string(i) + "]", lights[i].Position);
            defer_lighting_pass_shader->setVec3("lightColor[" + std::to_string(i) + "]", lights[i].color);
        }
        defer_lighting_pass_shader->setMat4("view", Tool::camera.GetViewMatrix());
        defer_lighting_pass_shader->setMat4("projection", Tool::camera.GetCameraPerspective());
        
        //PrepareLUT()已经写过quadVAO，直接拿来用
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        #pragma endregion
    }
    #pragma endregion
}
void Scene::DrawScene2GBuffer()
{
    
}

void Scene::DrawPlaneDepth()
{
    glViewport(0, 0, 1024, 1024);
    drawDepthShader->use();
    drawDepthShader->setMat4("model", plane.m_model);
    for(int i = 0; i < lights.size(); i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBOs[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        drawDepthShader->setMat4("lightVP", lights[i].LightVP);
        glBindVertexArray(plane.m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene::DrawModelDepth()
{
    glViewport(0, 0, 1024, 1024);
    //glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    drawDepthShader->use();
    for(int l = 0; l < lights.size(); l++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBOs[l]);
        //glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMapIDs[l], 0);
        drawDepthShader->setMat4("lightVP", lights[l].LightVP);
        for(int i = 0; i < models.size(); i++)
        {
            drawDepthShader->setMat4("model", model4models[i]);
            for(auto mesh : models[i].meshes)
            {
                //逐模型逐网格遍历
                glBindVertexArray(mesh.VAO);
                glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
            }
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene::DrawPlane()
{
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#pragma region 前向渲染
    if(scene_render_mode == USE_FORWARD_RENDERING)
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
        drawPlaneShader->setMat4("model", plane.m_model);
        glm::mat4 view = Tool::camera.GetViewMatrix();
        glm::mat4 perspective = Tool::camera.GetCameraPerspective();
        drawPlaneShader->setMat4("view", view);
        drawPlaneShader->setMat4("prospective", perspective);
        drawPlaneShader->setVec3("viewPos", Tool::camera.Position);
        for(int i = 0; i < lights.size(); i++)
        {
            drawPlaneShader->setMat4("lightVP[" + std::to_string(i) + "]", lights[i].LightVP);
            drawPlaneShader->setVec3("lightPos[" + std::to_string(i) + "]", lights[i].Position);
            drawPlaneShader->setTexture("shadowMap[" + std::to_string(i) + "]", depthMapIDs[i], 2 + i);
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
#pragma endregion
#pragma region 延迟渲染
    if(scene_render_mode == USE_DEFERED_RENDERING)
    {
        if(multi_light_plane_shader == nullptr)
        {
            string vsPath = m_root_dir + "Shader/multilight_plane_vs.glsl";
            string fsPath = m_root_dir + "Shader/multilight_plane_fs.glsl";
            multi_light_plane_shader = new Shader(vsPath.c_str(), fsPath.c_str());
        }
        if(plane.m_textureID == -1)
        {
            plane.loadGenModelTexture();
        }
        glBindVertexArray(plane.m_VAO);
        multi_light_plane_shader->use();
        for(int i = 0; i < lights.size(); i++)
        {
            multi_light_plane_shader->setVec3("lightPos[" + std::to_string(i) + "]", lights[i].Position);
            multi_light_plane_shader->setVec3("lightColor[" + std::to_string(i) + "]", lights[i].color);
        }
        multi_light_plane_shader->setTexture("planeTexture", plane.m_textureID, 1);
        multi_light_plane_shader->setMat4("model", plane.m_model);
        glm::mat4 view = Tool::camera.GetViewMatrix();
        glm::mat4 perspective = Tool::camera.GetCameraPerspective();
        multi_light_plane_shader->setMat4("view", view);
        multi_light_plane_shader->setMat4("perspective", perspective);
        multi_light_plane_shader->setVec3("viewPos", Tool::camera.Position);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
#pragma endregion
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

