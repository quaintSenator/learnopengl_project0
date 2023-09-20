#ifndef MESH_H
#define MESH_H

#include "../Model/Model.h"
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Shader/Shader.h"

#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    int textureBitMask;
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        textureBitMask = 0b1111;
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
        std::cout << "VAO = " << VAO << " VBO = " << VBO << " EBO = " << EBO << std::endl;
    }
    Mesh(int texBitMask)
    {
        textures = vector<Texture>(4);
        textureBitMask = texBitMask;
        Build_generated_model_plane();
        std::cout << "Generated Mesh, VAO = " << VAO << std::endl;
    }

    void Build_generated_model_plane()
    {
        #pragma region 生成顶点配置VAO&VBO
        std::vector<float> planeVertices = {
            // positions            // normals         // texcoords     //tangent         //bitangent=NXT
            5.0f, -0.013f,  5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,     0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
           -5.0f, -0.013f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,     0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
           -5.0f, -0.013f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,     0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
            5.0f, -0.013f,  5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,     0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
           -5.0f, -0.013f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,     0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
            5.0f, -0.013f, -5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  5.0f,     0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f
        };
        glGenVertexArrays(1, &this->VAO);
        glBindVertexArray(this->VAO);
        glGenBuffers(1, &this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), &planeVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
        
        glBindVertexArray(0);
        #pragma endregion
        #pragma region 写入textures但未load
        Texture diffuse_texture = Texture();
        Texture normal_texture = Texture();
        diffuse_texture.path = "D:/learnOpenGLRepo/learnopengl_project0/learnopengl_project0/textures/brickwall.jpg";
        normal_texture.path = "D:/learnOpenGLRepo/learnopengl_project0/learnopengl_project0/textures/brickwall_normal.jpg";
        textures.resize(2);
        textures[0] = diffuse_texture;
        textures[1] = normal_texture;
        #pragma endregion 
    }
    void Draw2depth(Shader& depthShader){
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    int hasDiffuse()
    {
        return (textureBitMask & 0b1000) >> 3;
    }
    int hasNormal()
    {
        return (textureBitMask & 0b0100) >> 2;
    }
    int hasMaskMap()
    {
        return (textureBitMask & 0b0010) >> 1;
    }
    int hasAO()
    {
        return (textureBitMask & 0b0001);
    }
    void Draw(Shader& shader, bool isFBX, bool isDefered = false)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        
        shader.use();
        //shader.setMat4("model", model);
        if(!isFBX)
        {
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                string number;
                string name = textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to string
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to string
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to string
                // now set the sampler to the correct texture unit
                //glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
                shader.setInt(name + number, i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                //Tool::nextGeneratableTextureID++;
            }
        }
        else // FBX 
        {
            if(hasDiffuse())
            {
                shader.setTexture(string("texture_diffuse1"), textures[0].id, 0);
            }
            if(hasNormal())
            {
                shader.setTexture(string("texture_normal1"), textures[1].id, 1);
            }
            if(hasMaskMap())
            {
                shader.setTexture(string("texture_maskmap"), textures[2].id, 2);
            }
            if(hasAO())
            {
                shader.setTexture(string("texture_AO1"), textures[3].id, 3);
            }
            
        }
        if(isDefered)
        {
            
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif