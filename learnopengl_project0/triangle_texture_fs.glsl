#version 330 core
out vec4 FragColor;
in vec3 interpolatedVertColor;
in vec2 uv;
uniform sampler2D ourTexture1;

void main()
{
    vec4 sampledColor = texture(ourTexture1,uv);
    vec4 mixColor = mix(sampledColor,vec4(interpolatedVertColor,1.0),0.5);
	FragColor = mixColor;
}