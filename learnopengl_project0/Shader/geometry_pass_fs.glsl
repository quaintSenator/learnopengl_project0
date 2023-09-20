#version 330 core
out vec4 FragColor;
layout (location = 0) out vec4 G_FragPos;
layout (location = 1) out vec3 G_Normal;
layout (location = 2) out vec3 G_DiffuseColor;
layout (location = 3) out vec3 G_Ao;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_maskmap;
uniform sampler2D texture_AO1;

in VS_OUT{
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
    vec3 V_Normal;
    vec2 uv;
} fs_in;
uniform mat4 model;

const float NEAR = 0.1;
const float FAR = 100.0;
float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR)/ (FAR + NEAR - z * (FAR - NEAR));
}//线性深度在[0, 1]

void main()
{   
    G_FragPos = vec4(fs_in.FragPos, gl_FragCoord.z);
    
    //我们在G_Normal直接写入转换完成的精法线
    vec3 normal = normalize(texture(texture_normal1, fs_in.uv)).xyz;//切线空间
    vec3 T = normalize(vec3(model * vec4(fs_in.Tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(fs_in.V_Normal, 0.0)));
    vec3 B = cross(T, N);
    /*
    vec3 T = fs_in.Tangent;
    vec3 B = fs_in.Bitangent;
    vec3 N = fs_in.V_Normal;//粗法线，世界坐标
    */
    mat3 TBN = mat3(T, B, N);
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    
    //G_Normal = mat3(model) * G_Normal;
    G_Normal = normal;
    
    G_DiffuseColor = texture(texture_diffuse1, fs_in.uv).xyz;
    G_Ao.r = texture(texture_AO1, fs_in.uv).r;
    G_Ao.g = texture(texture_maskmap, fs_in.uv).r;
}