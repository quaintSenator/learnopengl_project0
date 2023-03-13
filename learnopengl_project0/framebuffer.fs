#version 330 core

in vec2 aTexCoord;
out vec4 FragColor;

uniform sampler2D texturesamp;
uniform float heaviness;
 
void main()
{
    float offset = 1.0 / heaviness; 
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );
    float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);
    vec3 samplist[9];
    for(int i = 0 ; i < 9; i++){
        samplist[i] = texture(texturesamp, aTexCoord + offsets[i]).xyz;
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++){
        col += samplist[i] * kernel[i];
    }
    FragColor = vec4(col, 1.0);

}