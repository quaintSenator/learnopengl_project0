#version 330 core
in VS_OUT{
	vec3 FragPos;//这是顶点的真实位置或者说原始位置
	vec3 Normal;//这是校准后的法线
	vec2 TexCoords;//这是直接插值得到的uv
	vec4 FragLightSpacePos;//这是shading point在光源空间的映射
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

float calcShadow(vec4 fragPosLightSpace){

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//这一步被称为透视除法，事实上，按照齐次坐标的定义，向量(x,y,z,w)和（2x,2y,2z,2w）是完全一样的
	//however，这一步我们往往是在gl_Position = mvp(...)中隐式地完成的
	//这一步计算出的gl_Position如果w≠1，openGL会帮助我们进行透视除法; 而fragPosLightSpace是我们自己建立的变量，不会自动执行
	//在透视变换 + 透视除法 之后，所有顶点的xyz将会位于NDC中
	projCoords = (projCoords + 1.0) / 2; //把[-1,1]再映射到[0,1]，注意深度纹理的uv是[0,1],深度也是[0,1]

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
	//blinn-phong部分的所有点，都是真实坐标空间进行的运算，仅仅进行model变换而不进行VP变换
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
