#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 aNormal;
layout(location = 2)in vec2 aTexCoords;

uniform mat4 lightVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragLightSpacePos;
} vs_out;
void main(){
	vs_out.FragPos = (model * vec4(aPos, 1.0)).xyz;//这是顶点的真实位置或者说原始位置
	//要乘上model是因为我们实际上用了同一批顶点绘制了多次，分别用了不同的model
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	//为什么要做这一步，可以参考我在笔记本
	//https://github.com/quaintSenator/All_note/blob/master/%E5%85%AB%E8%82%A1%E5%BF%AB%E6%8D%B7%E7%AC%94%E8%AE%B0/%E5%A4%87%E6%88%98%E9%A2%98%E5%BA%93-%E9%9A%8F%E6%83%B3%E9%9A%8F%E5%86%99.md#%E5%9B%BE%E5%BD%A2%E5%AD%A6%E6%95%B0%E5%AD%A6
	//或者搜索”什么是法线矩阵“找寻答案
	vs_out.TexCoords = aTexCoords;
	vs_out.FragLightSpacePos = lightVP * vec4(vs_out.FragPos, 1.0);

	gl_Position = projection * view * model * vec4(aPos, 1.0);//这是屏幕空间位置,传递给2pass fs就可以在屏幕上绘制

}