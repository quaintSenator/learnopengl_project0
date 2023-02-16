#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};
struct Light {
	glm::vec3 lightPos;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 0.05f;
float cameraAngularSpeed = 0.015f;
float lastX = 400, lastY = 300;
float mouseSensitive = 0.005f;

float pitch = 0.0f;
float yaw = 0.0f;
float roll = 0.0f;

glm::vec3 getCameraFront() {
	return glm::vec3(cos(yaw) * cos(pitch), sin(pitch), cos(pitch) * sin(yaw));
}
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//glm::vec3 right = glm::cross(cameraFront, cameraUp);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		yaw -= cameraAngularSpeed;
		cameraFront = getCameraFront();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		yaw += cameraAngularSpeed;
		cameraFront = getCameraFront();
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraFront * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraFront * cameraSpeed;
	}
	

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	
	float xoffset = (xpos - lastX) * mouseSensitive;
	float yoffset = (ypos - lastY) * mouseSensitive;
	lastX = xpos;
	lastY = ypos;
	
	yaw += xoffset;
	pitch -= yoffset;//pitch上为大，y下为大

	cameraFront = getCameraFront();
	cameraFront = glm::normalize(cameraFront);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void printMat4(float* p) {
	for (int i = 0; i < 16; i++) {
		std::cout << *p << " ";
		p++;
		if ((i + 1) % 4 == 0)std::cout << std::endl;
	}
}

void printVec3(float* p) {
	for (int i = 0; i < 3; i++) {
		std::cout << *p << "";
		p++;
		std::cout << std::endl;
	}
}




int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "repeat yesterday", NULL, NULL);
	if (!window) {
		std::cout << "Window Create failed" << std::endl;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glEnable(GL_DEPTH_TEST);

	Shader lightShader("./colors.vs", "./colors.fs");

	Shader lightCubeShader("./light_cube.vs", "./light_cube.fs");

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	unsigned int cubeVAO, VBO;
	unsigned int lightCubeVAO;

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glm::vec3 ma(1.0f, 0.5f, 0.31f);
	glm::vec3 md(1.0f, 0.5f, 0.31f);
	glm::vec3 ms(0.5f, 0.5f, 0.5f);
	float shininess = 32.0f;

	glm::vec3 la(0.2f, 0.2f, 0.2f);
	glm::vec3 ld(0.5f, 0.5f, 0.5f);
	glm::vec3 ls(1.0f, 1.0f, 1.0f);

	lightShader.use();
	glUniform3fv(glGetUniformLocation(lightShader.ID, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(lightShader.ID, "viewPos"), 1, glm::value_ptr(cameraPos));

	glUniform3fv(glGetUniformLocation(lightShader.ID, "material.ambient"), 1, glm::value_ptr(ma));
	glUniform3fv(glGetUniformLocation(lightShader.ID, "material.diffuse"), 1, glm::value_ptr(md));
	glUniform3fv(glGetUniformLocation(lightShader.ID, "material.specular"), 1, glm::value_ptr(ms));
	glUniform1f(glGetUniformLocation(lightShader.ID, "material.shininess"), shininess);

	glUniform3fv(glGetUniformLocation(lightShader.ID, "light.ambient"), 1, glm::value_ptr(la));
	glUniform3fv(glGetUniformLocation(lightShader.ID, "light.diffuse"), 1, glm::value_ptr(ld));
	glUniform3fv(glGetUniformLocation(lightShader.ID, "light.specular"), 1, glm::value_ptr(ls));
	glUniform3fv(glGetUniformLocation(lightShader.ID, "light.lightPos"), 1, glm::value_ptr(lightPos));

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightShader.use();
		glBindVertexArray(cubeVAO);

	
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 prospective = glm::mat4(1.0f);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		prospective = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "prospective"), 1, GL_FALSE, &prospective[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(lightCubeVAO);
		lightCubeShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "prospective"), 1, GL_FALSE, &prospective[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}



