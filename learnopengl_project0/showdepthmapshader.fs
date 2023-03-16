#version 330 core

in vec2 texCoords;
uniform sampler2D depthMap;
uniform float near;
uniform float far;
out vec4 FragColor;

float LinearDepth(float depth)
{
	//here d is a sample from depthMap whose range is [0.0, 1.0], and has been delineared
	//(see https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/01%20Depth%20testing/#_3)
	//we need to map it back to NDC, 

    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
void main(){
	float depthValue = texture(depthMap, texCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0);
	//float z = texture(depthMap, texCoords).r;//深度纹理第一维就是深度
	//FragColor = vec4(vec3(LinearDepth(z)), 1.0);
	//这个线性函数是learnopenGL代码仓库的写法貌似有问题哈 一直出纯白图 我注释掉了 还是用原来的直接采样函数
}