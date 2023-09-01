
#ifndef GENERATED_MODEL_H
#define GENERATED_MODEL_H

#include "../Shader/Shader.h"
#include <vector>
#include "../Model/Model.h"
#include "../m_Tool/Tool.h"
#include "../main.h"


enum GeneratedModelType
{
    Plane,
    Sphere
};

class GeneratedModel
{
public:
    glm::mat4 m_model;
    unsigned int m_VAO = -1;
    unsigned int m_textureID = -1;
    //Mesh* generatedMesh;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    
    GeneratedModel(glm::mat4 model) : m_model(model)
    {
        /*if(type == GeneratedModelType::Plane)
        {
            vertices = vector<Vertex>(6);
            vertices[0].Position = glm::vec3(2.0f, -0.013f,  2.0f);
            vertices[1].Position = glm::vec3(-2.0f, -0.013f,  2.0f);
            vertices[2].Position = glm::vec3(-2.0f, -0.013f, -2.0f);
            vertices[3].Position = glm::vec3( 2.0f, -0.013f,  2.0f);
            vertices[4].Position = glm::vec3( -2.0f, -0.013f, -2.0f);
            vertices[5].Position = glm::vec3(2.0f, -0.013f, -2.0f);
            for(int i = 0; i < 6; i++)
            {
                vertices[i].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            vertices[0].TexCoords = glm::vec2(5.0f,  0.0f);
            vertices[1].TexCoords = glm::vec2(0.0f,  0.0f);
            vertices[2].TexCoords = glm::vec2(0.0f,  5.0f);
            vertices[3].TexCoords = glm::vec2(5.0f,  0.0f);
            vertices[4].TexCoords = glm::vec2(0.0f,  5.0f);
            vertices[5].TexCoords = glm::vec2(5.0f,  5.0f);

            indices = {
                1, 2, 3,
                4, 5, 6
            };
            textures = vector<Texture>();
            generatedMesh = new Mesh(vertices, indices, textures);
        }*/
        std::vector<float> planeVertices = {
            // positions            // normals         // texcoords
            5.0f, -0.013f,  5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
           -5.0f, -0.013f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
           -5.0f, -0.013f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
            5.0f, -0.013f,  5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
           -5.0f, -0.013f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
            5.0f, -0.013f, -5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  5.0f
        };
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), &planeVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        
        glBindVertexArray(0);
    }
    GeneratedModel() = default;
    ~GeneratedModel()
    {
        //delete generatedMesh;
    }
    
    //std::string planeTextureDirectory = "E:/workplace/vsworkplace/learnopengl_project0/learnopengl_project0/textures";
    std::string planeTextureDirectory = m_root_dir + "textures";
    std::string planeTextureFilename = "wood.png";
    void loadGenModelTexture()
    {
        m_textureID = Model::TextureFromFile(planeTextureFilename.c_str(), planeTextureDirectory);
    }
    
private:
    unsigned int m_VBO;
    
    void drawGeneratedModel(Shader& shader)
    {
        
    }
};

#pragma region 垃圾桶

#pragma endregion

#endif