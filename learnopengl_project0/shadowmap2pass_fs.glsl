#version 330 core
in VS_OUT{
	vec3 FragPos;//���Ƕ������ʵλ�û���˵ԭʼλ��
	vec3 Normal;//����У׼��ķ���
	vec2 TexCoords;//����ֱ�Ӳ�ֵ�õ���uv
	vec4 FragLightSpacePos;//����shading point�ڹ�Դ�ռ��ӳ��
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

float calcShadow(vec4 fragPosLightSpace){

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//��һ������Ϊ͸�ӳ�������ʵ�ϣ������������Ķ��壬����(x,y,z,w)�ͣ�2x,2y,2z,2w������ȫһ����
	//however����һ��������������gl_Position = mvp(...)����ʽ����ɵ�
	//��һ���������gl_Position���w��1��openGL��������ǽ���͸�ӳ���; ��fragPosLightSpace�������Լ������ı����������Զ�ִ��
	//��͸�ӱ任 + ͸�ӳ��� ֮�����ж����xyz����λ��NDC��
	projCoords = (projCoords + 1.0) / 2; //��[-1,1]��ӳ�䵽[0,1]��ע����������uv��[0,1],���Ҳ��[0,1]

	if(projCoords.z > 1.0){
		return 0.0;
	}
		
	float bias = 0.005;
	float recordedDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth - recordedDepth > bias ? 1.0 : 0.0;

	return shadow;
}
void main(){
	//blinn-phong���ֵ����е㣬������ʵ����ռ���е����㣬��������model�任��������VP�任
	vec3 matColor = texture(diffuseTexture, fs_in.TexCoords).xyz;
	vec3 ambientTerm = 0.3  * matColor;
	vec3 l = normalize(lightPos - fs_in.FragPos);
	vec3 n = normalize(fs_in.Normal);
	vec3 e = normalize(viewPos - fs_in.FragPos);
	vec3 h = normalize(l + e);
	float hdotn = max(dot(h, n), 0);
	float ldotn = max(dot(l, n), 0);
	float shininess = 64.0;

	vec3 specularTerm = matColor * pow(hdotn, shininess);

	vec3 diffuseTerm = matColor * ldotn;

	//FragColor = vec4(ambientTerm + specularTerm + diffuseTerm, 1.0);
	FragColor = vec4(ambientTerm + (specularTerm + diffuseTerm) * (1.0 - calcShadow(fs_in.FragLightSpacePos)), 1.0);

}
