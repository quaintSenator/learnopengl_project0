#include "./m_Tool/Tool.h"
#include "main.h"
#include <string>
#include "Scene/GeneratedModel.h"

// settings
extern const unsigned int SCR_WIDTH = 1920;
extern const unsigned int SCR_HEIGHT = 1061;
const unsigned int sphere_x_segment = 64;
const unsigned int sphere_y_segment = 64;
const float PI = 3.14159265359;
/*extern float deltaTime;
extern float lastFrame;
extern float currentFrame;*/

int main()
{
    Tool::InitWindow(SCR_WIDTH,SCR_HEIGHT);
    m_root_dir = "D:/learnOpenGLRepo/learnopengl_project0/learnopengl_project0/";
    Tool::RenderLoop();
    Tool::Terminate();
    return 0;
} 

/*int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("./model_loading_vs.glsl", "./model_loading_fs.glsl");

    // load models
    // -----------
    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model ourModel("E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/nanosuit/nanosuit.obj");

    ourModel.Draw(ourShader);
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera.SetTarget(glm::vec3(0.0, 0.0, 0.0));
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        glm::vec3 lightPos = glm::vec3(1.2, 0.0, 1.0);
        glm::vec3 cameraPos = camera.Position;
        ourShader.setVec3("lightPos",lightPos);
        ourShader.setVec3("cameraPos",cameraPos);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}*/


/*void process_shader(int shader_name, GLFWwindow* window)
{
    switch(shader_name)
    {
        case PBR_DIRECT_LIGHT_SHADER:
            process_pbr_direct_light_shader(window,camera);
            break;
        case DRAW_1_TRIANGLE:
            process_draw_1_triangle_shader(window);
            break;
        case DRAW_1_TRIANGLE_WITH_EBO:
            process_draw_1_triangle_withEBO(window);
            break;
        case TRIANGLE_TIME_SHIFT_COLOR:
            process_triangle_time_shift_color(window);
            break;
        case TRIANGLE_COLOR_INTERPOLATION:
            process_triangle_color_interpolation(window);
            break;
        case RECTANGLE_TEXTURE:
            process_draw_1_triangle_withTexture(window);
            break;
        case RECTANGLE_2_TEXTURES:
            process_draw_rectangle_with2Textures(window);
            break;
        case DRAW_CUBE_NO_ZBUFFER:
            process_draw_cube_noZbuffer(window);
            break;
        case DRAW_CUBE_ZBUFFER:
            process_spinning_cube_Zbuffer(window);
            break;
        case BASIC_ILLUMINATION:
            process_basic_illumination(window,camera);
            break;
        case PHONG_SHADING:
            process_phong_shading(window,camera);
            break;
        case SHADOW_MAP:
            process_shadowmap(window,camera);
            break;
        default:
            break;
    }
}*/

// utility function for loading a 2D texture from file
// ---------------------------------------------------
/*
void renderScene(const Shader& shader,unsigned int &cubeVAO, unsigned int &cubeVBO, unsigned int &planeVAO)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    float y = 2 + 2 * sin(glfwGetTime());
    model = glm::translate(model, glm::vec3(0.0f, y, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube(cubeVAO, cubeVBO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube(cubeVAO, cubeVBO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube(cubeVAO, cubeVBO);
}
void renderCube(unsigned int &cubeVAO, unsigned int &cubeVBO)
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            //position           //normal             //uv
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void renderSphere(const Shader& shader, unsigned int &sphereVAO, unsigned int& sphereVBO, unsigned int& sphereEBO, unsigned int &indexCount)
{
    // initialize (if necessary)
    if (sphereVAO == 0)
    {
        //这种做法是从单位球上的球坐标而来的，原理简单说就是把球坐标仰角α 64等分，再把转角θ 64等分
        //如此组合起来有64 * 64个方向(球心出发)，每个方向与单位球的交点就是我们需要的顶点
        //这种分法会导致球的南极和北极区域点非常密集，而赤道区域相对稀疏
        std::vector<float> verts;
        glm::vec3 vertice;
        glm::vec3 normal;
        glm::vec2 uv;
        for (int i = 0; i <= sphere_x_segment; i++) {
            for (int j = 0; j <= sphere_y_segment; j++) {
                float xseg = (float)i / (float)sphere_x_segment;
                float yseg = (float)j / (float)sphere_y_segment;
                
                float xPos = cos(2.0f * PI * xseg) * sin(yseg * PI);
                float yPos = cos(PI * yseg);
                float zPos = sin(xseg * 2.0f * PI) * sin(yseg * PI);
                
                vertice = glm::vec3(xPos, yPos, zPos);//从原点也就是球心指向迭代当前点，就是normal
                normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
                uv = glm::vec2(xseg, yseg);//其实这也正是球表面uv采样的规范：[0,0]，[0,1/64],...[64/64, 64/64]

                verts.push_back(vertice.x);
                verts.push_back(vertice.y);
                verts.push_back(vertice.z);
                verts.push_back(normal.x);
                verts.push_back(normal.y);
                verts.push_back(normal.z);
                verts.push_back(uv.x);
                verts.push_back(uv.y);
            }
        }
        //顶点数量是64 * 64
        bool isOdd = false;
        std::vector<unsigned int> indices;

        //假如在平面上，上一条线有64个点，下一条线上有64个点，把他们全都连接成三角形条带：
        for (unsigned int y = 0; y < sphere_y_segment; ++y)
        {
            if (!isOdd) // even rows: y == 0, y == 2; 
            {
                for (unsigned int x = 0; x <= sphere_x_segment; ++x)
                {
                    indices.push_back(y * (sphere_x_segment + 1) + x);
                    indices.push_back((y + 1) * (sphere_x_segment + 1) + x);
                    //0,65;1,66;2,67;......64,129
                    //这是一段锯齿状的点阵：在纬度0的环和纬度1的环上，每个环64个点，这个顺序
                }
            }
            else
            {
                for (int x = sphere_x_segment; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (sphere_x_segment + 1) + x);
                    indices.push_back(y * (sphere_x_segment + 1) + x);
                    //130+64,65+64;130+63,65+63......130,65
                }
            }
            isOdd = !isOdd;
        }
        
        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);
        glBindVertexArray(sphereVAO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size()* sizeof(float), &verts[0], GL_STATIC_DRAW);
        //fill index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        // link vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }
    // render
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    //GL_TRIANGLE_STRIP 表示，我们将需要绘制三角形条带
    //
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly

*/