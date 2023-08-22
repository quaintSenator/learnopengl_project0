
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "main.h"

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

void process_draw_1_triangle_shader(GLFWwindow* window)
{
    glDisable(GL_DEPTH_TEST);
    
    /*const char* vsPath = "./draw1trianglevs.txt";
    const char* fsPath = "./draw1trianglefs.txt";*/
    const char* vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/draw1trianglevs.glsl";
    const char* fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/draw1trianglefs.glsl";
    Shader ourShader(vsPath,fsPath);
    ourShader.use();
    float vertices[] = {
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f,    // top
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //pass uniform mvp matrix to shader
        /*glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom,(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);
        glm::mat4 mvp = model*view*projection;*/
        
        //ourShader.setMat4("mvp",mvp);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}