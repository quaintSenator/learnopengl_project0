#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
void main()
{
	//FragColor = mix(texture(ourTexture1, TexCoord),texture(ourTexture2, TexCoord),0.2);
	FragColor = vec4(objectColor * lightColor, 1.0);
}