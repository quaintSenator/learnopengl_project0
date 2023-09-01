#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 cameraPos;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 worldNormal;
}vs_out;

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	
	vs_out.FragPos = gl_Position.xyz;
	vs_out.TexCoord = aTexCoord;
	
	mat3 NormalMatrix = transpose(inverse(mat3(model)));
	
	vec3 T = normalize(NormalMatrix * Tangent);
	vec3 B = normalize(NormalMatrix * Bitangent);
	vec3 N = normalize(NormalMatrix * aNormal);
	
	mat3 TBN = transpose(mat3(T, B, N));//正交矩阵，转置 = 求逆
	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * cameraPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;
	mat3 noTranslateModel = mat3(model);
	vs_out.worldNormal = noTranslateModel * aNormal;
	
	//Normal = mat3(model) * aNormal;
	//TexCoord = aTexCoord;
}