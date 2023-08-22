#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "main.h"
#include "camera.h"

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;


void process_pbr_direct_light_shader(GLFWwindow* window,Camera& camera)
{
    //const std::string pbrass = "D:/Cgworkplace/learnopenGLsource/LearnOpenGL/resources/textures/pbr/rusted_iron";
    Shader pbrdirectlightShader("./pbrdirectlight.vs", "./pbrdirectlight.fs");
    Shader sphereExampleShader("./sphereExample.vs", "./sphereExample.fs");
    // build and compile shaders
    // -------------------------
    unsigned int sphereVAO = 0;
    unsigned int sphereVBO = 0;
    unsigned int sphereEBO = 0;
    unsigned int indexCount = 0;
    // render loop
    unsigned int nrRows = 6;
    unsigned int nrColumns = 6;
    float spacing = 2.5f;

    pbrdirectlightShader.use();
    
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        pbrdirectlightShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        pbrdirectlightShader.setVec3("cameraPos", camera.Position);
        pbrdirectlightShader.setMat4("view", view);
        pbrdirectlightShader.setMat4("projection", projection);
        pbrdirectlightShader.setFloat("ao", 1.0f);
        pbrdirectlightShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3, 0.2, 0.1, 1.0);

        sphereExampleShader.use();
        sphereExampleShader.setMat4("view", view);
        sphereExampleShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; ++i)
        {
            glm::vec3 newPos = lightPositions[i];
            pbrdirectlightShader.use();
            pbrdirectlightShader.setVec3("lightPos[" + std::to_string(i) + "]", newPos);
            pbrdirectlightShader.setVec3("lightColor[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            sphereExampleShader.use();
            sphereExampleShader.setMat4("model", model);

            renderSphere(sphereExampleShader, sphereVAO, sphereVBO, sphereEBO, indexCount);
        }

        pbrdirectlightShader.use();
        for (int row = 0; row < nrRows; row++)
        {
            pbrdirectlightShader.setFloat("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; col++)
            {
                pbrdirectlightShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                //clamp区间平移：arg1大于上界返回上界，小于下界返回下界，否则返回arg1
                //roughness = 0.0表示完美光滑表面，在直接光下看起来不太好

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - ((float)nrColumns / 2.0)) * spacing,
                    (row - ((float)nrRows / 2.0)) * spacing,
                    //(row + 1) * (col + 1)
                    -3.0
                ));
                pbrdirectlightShader.setMat4("model", model);
                renderSphere(pbrdirectlightShader, sphereVAO, sphereVBO, sphereEBO, indexCount);
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
}