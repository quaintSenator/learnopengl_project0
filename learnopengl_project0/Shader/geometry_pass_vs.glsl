#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;


out VS_OUT{
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
    vec3 V_Normal;
    vec2 uv;
} vs_out;
uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{   
    gl_Position = perspective * view * model * vec4(aPos, 1.0f);
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
    vs_out.uv = aTexCoord;
    
    //mat4 mv = view * model;
    /*mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.V_Normal = normalize(normalMatrix * aNormal);
    vs_out.Tangent = normalize(normalMatrix * Tangent);
    vs_out.Bitangent = normalize(normalMatrix * Bitangent);*/
    vs_out.V_Normal = normalize(mat3(model) * aNormal);
    vs_out.Tangent = normalize(mat3(model) * Tangent);
    vs_out.Bitangent = normalize(mat3(model) * Bitangent);
}