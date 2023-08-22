#version 330 core
out vec4 FragColor;
uniform float time;
void main()
{
	//FragColor = mix(texture(ourTexture1, TexCoord),texture(ourTexture2, TexCoord),0.2);
	float sinTime = (sin(time) + 1)/2;
	vec3 color = vec3(sinTime,1-sinTime,1);
	FragColor = vec4(color,1.0);
}