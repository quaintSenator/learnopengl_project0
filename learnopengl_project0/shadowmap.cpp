#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "main.h"

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern float deltaTime;
extern float lastFrame;
extern float currentFrame;

void process_shadowmap(GLFWwindow* window,Camera& camera)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
    const char* vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/box_vs.glsl";
    const char* fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/box_fs.glsl";
	Shader lightCubeShader(vsPath, fsPath);
	
	vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/shadowmap1pass_vs.glsl";
	fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/shadowmap1pass_fs.glsl";
	Shader onePassShader(vsPath, fsPath);
	
	vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/shadowmap2pass_vs.glsl";
	fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/shadowmap2pass_fs.glsl";
	Shader twoPassShader(vsPath, fsPath);
	
	//这个程序的逻辑说明：我们先设定一个自定义的framebuffer，第一遍渲染我们不渲染到默认fbo，而是渲染给
	//这个自定义的fbo，这样实际的渲染结果就存到了fbo texture attachment 当中，这次渲染对应光源视角
	//然后我们再恢复默认fbo，再渲染一次，
	//二次渲染的fs内容就是从texture直接采样

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	onePassShader.use();

	unsigned int planeVAO;
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};
	// plane VAO
	unsigned int cubeVAO = 0, cubeVBO = 0;
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

	unsigned int quadVAO;
	unsigned int quadVBO;
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// setup quad plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	
	// load textures
	unsigned int woodTexture = loadTexture("D:/Cgworkplace/learnopenGLsource/LearnOpenGL/resources/textures/wood.png");

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//onePassShader.use();
	//onePassShader.setInt("texture1", 0);
	//twoPassShader.use();
	//twoPassShader.setInt("texturesamp", 0);

	//generate frame buffer for depthmap
	unsigned int depthMapfbo;
	glGenFramebuffers(1, &depthMapfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapfbo);

	unsigned int depthMapTexture;
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;//一张1024*1024的shadowmap,未必需要和屏幕等大
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//这里使用clamptoborder能解决两个问题：1在深度纹理之外的范围有多个阴影区域的问题，这是因为那里repeat到了三个盒子留下的深度位置
	//2 在平台极远处有些点落在深度贴图之外，他们的深度按照原本算法将会大于边界值，现在边界值是最大值，远处不进行阴影
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	//自定义的framebuffer绑定了一个深度纹理depthmap
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//一般，如果一个fbo没有绑定颜色缓冲，是无法通过完备性检测的，我们可以通过上面两句话告诉openGL这个fbo不必绑定颜色缓冲，因为我们本来就
	//不打算往里面绘制颜色
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "FrameBuffer Complete" << std::endl;
	}
	else {
		std::cout << "Frame buffer not Complete" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//这个帧缓冲是我们自己定义的帧缓冲，应当要求其让位，免得占据了默认帧缓冲的绑定态，从而让最终屏幕画不出东西

	glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		// 1 pass render: write to depth map
		// ------
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapfbo);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		onePassShader.use();
		//lightPos.y = 2 * sin(currentFrame) + 2;
		float near = 1.0f, far = 40.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
		glm::mat4 lightview = glm::lookAt(lightPos, sceneCenter, up);
		glm::mat4 lightVP = lightProjection * lightview;
		onePassShader.setMat4("lightVP", lightVP);
		renderScene(onePassShader, cubeVAO, cubeVBO, planeVAO);
		//至此，depthMapfbo的深度附件上已经拥有了我们绘制好的shadowmap深度纹理
		//若要取用这个纹理，直接绑定depthMapTexture即可

		//2 pass render
		twoPassShader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);//回到默认fbo
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		twoPassShader.setInt("diffuseTexture", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		twoPassShader.setInt("shadowMap", 1);

		twoPassShader.setVec3("lightPos", lightPos);
		twoPassShader.setVec3("viewPos", camera.Position);
		twoPassShader.setMat4("lightVP", lightVP);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		twoPassShader.setMat4("view", view);
		twoPassShader.setMat4("projection", projection);


		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		renderScene(twoPassShader, cubeVAO, cubeVBO, planeVAO);

		lightCubeShader.use();
		glBindVertexArray(cubeVAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setMat4("model", model);
		lightCubeShader.setMat4("view", view);
		lightCubeShader.setMat4("projection", projection);
		renderCube(cubeVAO, cubeVBO);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}