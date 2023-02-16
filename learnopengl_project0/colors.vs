#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 prospective;

out vec3 Normal;
out vec3 FragPos;

void main(){
	gl_Position = prospective * view * model * vec4(aPos, 1.0);
	FragPos = (model * vec4(aPos, 1.0)).xyz;
	Normal = aNormal;
}