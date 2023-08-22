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

void process_basic_illumination(GLFWwindow* window,Camera& camera)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
    const char* vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/basic_illumination_vs.glsl";
    const char* fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/basic_illumination_fs.glsl";
	
	Shader lightShader(vsPath,fsPath);
	vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/lightCube_vs.glsl";
	fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/lightCube_fs.glsl";
	Shader lightCubeShader(vsPath,fsPath);
    lightShader.use();
#pragma region 顶点数据
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	 };
#pragma endregion
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	unsigned int cubeVAO, VBO;
	unsigned int lightCubeVAO;

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glm::vec3 objColor(1.0f, 0.5f, 0.31f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		
		processInput(window, camera);
		glm::vec3 cameraPos = camera.Position;
		glm::vec3 cameraFront = camera.Front;
		glm::vec3 cameraUp = camera.Up;
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region 1pass
		lightShader.use();
		glBindVertexArray(cubeVAO);
		lightShader.setVec3("objectColor",objColor);
		lightShader.setVec3("lightColor",lightColor);
		
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 prospective = glm::mat4(1.0f);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		prospective = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightShader.setMat4("model",model);
		lightShader.setMat4("view",view);
		lightShader.setMat4("prospective",prospective);
		glDrawArrays(GL_TRIANGLES, 0, 36);
#pragma endregion

#pragma region 2pass
		glBindVertexArray(lightCubeVAO);
		lightCubeShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		
		lightCubeShader.setMat4("model",model);
		lightCubeShader.setMat4("view",view);
		lightCubeShader.setMat4("prospective",prospective);
		glDrawArrays(GL_TRIANGLES, 0, 36);
#pragma endregion
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}