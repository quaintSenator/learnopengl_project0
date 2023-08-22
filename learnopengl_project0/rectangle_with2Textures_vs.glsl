#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 vertColor;
layout(location = 2)in vec2 TexCoord;

out vec3 interpolatedVertColor;
out vec2 uv;

void main()
{
	//gl_Position = mvp * vec4(aPos, 1.0);
	gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0);
	interpolatedVertColor = vertColor;
	uv = vec2(TexCoord.x,TexCoord.y);
}