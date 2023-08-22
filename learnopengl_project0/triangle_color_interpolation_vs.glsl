#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 vertColor;
out vec3 interpolatedVertColor;
uniform mat4 mvp;

void main()
{
	//gl_Position = mvp * vec4(aPos, 1.0);
	
	gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0);
	interpolatedVertColor = vertColor;
}