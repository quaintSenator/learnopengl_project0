#version 330 core
out vec4 FragColor;
in vec3 interpolatedVertColor;
in vec2 uv;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    vec4 sampledColor1 = texture(ourTexture1,uv);
    vec4 sampledColor2 = texture(ourTexture2,uv);
    
    vec4 mixColor = mix(sampledColor1,sampledColor2,0.5);
	FragColor = mixColor;
}