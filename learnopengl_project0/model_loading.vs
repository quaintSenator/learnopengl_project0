#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 pointPos;
out vec3 pointNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{   
    TexCoords = aTexCoords;   
    pointNormal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    pointPos = (model * vec4(aPos, 1.0)).xyz;
}