#version 330 core
out vec4 FragColor;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
in vec2 uv;
void main()
{
    vec4 tex1Color = texture(ourTexture1,uv);
    vec4 tex2Color = texture(ourTexture2,uv);
    
    vec4 mixedColor = mix(tex1Color,tex2Color,0.5);
    
	FragColor = mixedColor;
}