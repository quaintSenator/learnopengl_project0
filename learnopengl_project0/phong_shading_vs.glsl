#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 normal;

out vec3 i_normal;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 prospective;

void main()
{
	gl_Position = prospective * view * model * vec4(aPos, 1.0);
	i_normal = normal;
	FragPos = gl_Position.xyz;
}