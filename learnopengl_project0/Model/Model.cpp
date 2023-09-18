#include "Model.h"
#include "../main.h"


unsigned int Model::TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Texture Loaded Success,TextureID = " << textureID << std::endl;
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
unsigned int Model::TextureFromFile_CubeMap(const string& directory)
{

    unsigned int cubemapTextureID;
    glGenTextures(1, &cubemapTextureID);
    int width, height, nrComponents;
    unsigned char* data;

    vector<std::string> faces
    {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    bool isLoadSuccess = true;
    for(int i = 0; i < faces.size(); i++)
    {
        std::string file_FullDir = directory + '/' + faces[i];
        data = stbi_load(file_FullDir.c_str(), &width, &height, &nrComponents, 0);
        if(data)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
            
            GLenum format = GL_RGB;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            //glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            isLoadSuccess = false;
        }
        stbi_image_free(data);
    }
    if(isLoadSuccess)
    {
        std::cout << "Skybox Texture Loaded Success,TextureID = " << cubemapTextureID << std::endl;
    }
    else
    {
        std::cout << "Skybox Texture failed to load at path: " << std::endl;
    }
    return cubemapTextureID;
}
unsigned int Model::TextureFromFile_HDREnvMap(const char* path, const string& directory)
{
    string filename = string(path);
    filename = directory + filename;

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTextureID;
    if(data)
    {
        glGenTextures(1, &hdrTextureID);
        glBindTexture(GL_TEXTURE_2D, hdrTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
            GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        std::cout<< "HDR texture loaded Success, TextureID = " << hdrTextureID << std::endl;
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    stbi_image_free(data);
    return hdrTextureID;
}