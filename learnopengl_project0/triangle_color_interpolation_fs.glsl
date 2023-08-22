#version 330 core
out vec4 FragColor;
uniform float time;
in vec3 interpolatedVertColor;
void main()
{
	//FragColor = mix(texture(ourTexture1, TexCoord),texture(ourTexture2, TexCoord),0.2);

	FragColor = vec4(interpolatedVertColor,1.0);
}