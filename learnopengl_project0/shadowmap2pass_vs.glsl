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
	vs_out.FragPos = (model * vec4(aPos, 1.0)).xyz;//���Ƕ������ʵλ�û���˵ԭʼλ��
	//Ҫ����model����Ϊ����ʵ��������ͬһ����������˶�Σ��ֱ����˲�ͬ��model
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	//ΪʲôҪ����һ�������Բο����ڱʼǱ�
	//https://github.com/quaintSenator/All_note/blob/master/%E5%85%AB%E8%82%A1%E5%BF%AB%E6%8D%B7%E7%AC%94%E8%AE%B0/%E5%A4%87%E6%88%98%E9%A2%98%E5%BA%93-%E9%9A%8F%E6%83%B3%E9%9A%8F%E5%86%99.md#%E5%9B%BE%E5%BD%A2%E5%AD%A6%E6%95%B0%E5%AD%A6
	//����������ʲô�Ƿ��߾�����Ѱ��
	vs_out.TexCoords = aTexCoords;
	vs_out.FragLightSpacePos = lightVP * vec4(vs_out.FragPos, 1.0);

	gl_Position = projection * view * model * vec4(aPos, 1.0);//������Ļ�ռ�λ��,���ݸ�2pass fs�Ϳ�������Ļ�ϻ���

}