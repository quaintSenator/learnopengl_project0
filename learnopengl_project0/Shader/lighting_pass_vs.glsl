#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 Quad_uv;

uniform sampler2D G_FragPos;
out vec2 quad_uv;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    quad_uv = Quad_uv;
}