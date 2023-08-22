
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "main.h"

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

void process_triangle_time_shift_color(GLFWwindow* window)
{
    glDisable(GL_DEPTH_TEST);
    /*const char* vsPath = "./draw1trianglevs.txt";
    const char* fsPath = "./draw1trianglefs.txt";*/
    const char* vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/triangle_time_shift_color_vs.glsl";
    const char* fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/triangle_time_shift_color_fs.glsl";
    Shader ourShader(vsPath,fsPath);
    ourShader.use();
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ourShader.use();

        glBindVertexArray(VAO);
        float time = glfwGetTime();
        ourShader.setFloat("time",time);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}