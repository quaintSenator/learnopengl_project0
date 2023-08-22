
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "main.h"

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

void process_draw_1_triangle_withEBO(GLFWwindow* window)
{
    glDisable(GL_DEPTH_TEST);
    
    /*const char* vsPath = "./draw1trianglevs.txt";
    const char* fsPath = "./draw1trianglefs.txt";*/
    const char* vsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Ebo_draw1trianglevs_vs.glsl";
    const char* fsPath = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/Ebo_draw1trianglevs_fs.glsl";
    Shader ourShader(vsPath,fsPath);
    ourShader.use();
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
        };
    unsigned int indices[] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//用于执行线框绘制模式
    
    unsigned int VAO, VBO,EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ourShader.use();
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}