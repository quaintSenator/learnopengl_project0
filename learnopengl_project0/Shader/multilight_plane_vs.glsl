#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 TexCoord;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 uv;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{   
    gl_Position = perspective * view * model * vec4(aPos, 1.0f);
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
    vs_out.Normal = normal;
    //参见图形八股笔记 - 什么是法线矩阵，为什么要应用法线矩阵
    vs_out.uv = TexCoord;
}