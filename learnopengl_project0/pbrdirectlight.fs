#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 lightPos[4];
uniform vec3 lightColor[4];

uniform vec3 cameraPos;

const float PI = 3.14159265359;
vec3 Fresnel(float costheta, vec3 F0){
	//costheta = hdotv
	return F0 + (1.0 - F0) * pow(1.0 - costheta, 5.0);
}

float GGX(float costheta, float roughness){
	//costheta = ndotv
	float k = (roughness + 1) * (roughness + 1) / 8;
	//IBL : float k = roughness * roughness / 2;
	return costheta / (costheta * (1 - k) + k);
}

float NDF(float roughness, float costheta){ 
	//costheta = ndoth
	float division = costheta * costheta * (roughness * roughness - 1) + 1;
	division = division * division;
	return roughness * roughness / (3.1415926535 * division); 
}
void main(){
	vec3 L0 = vec3(0.0);
	vec3 n = normalize(Normal);
	vec3 v = normalize(cameraPos - WorldPos);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	float ndotv = max(dot(n, v), 0.0);
	vec3 flambert = albedo / PI;
	
	for(int i = 0; i < 4 ; i++){
		vec3 l = lightPos[i] - WorldPos;
		vec3 h = normalize(v + l);
		float ndoth = max(dot(n, h), 0.0);
		float hdotv = max(dot(h, v), 0.0);
		float ndotl = max(dot(n, l), 0.0);
		float distance = length(WorldPos - lightPos[i]);
		float attenuation = 1 / (distance * distance);
		vec3 radiance = lightColor[i] * attenuation;

		vec3 ks = Fresnel(hdotv, F0);
		vec3 kd = (1.0 - metallic) * (vec3(1.0) - ks);
		float fcooktorrance = GGX(ndotv, roughness) * NDF(roughness, ndoth) / (4.0 * ndotv * ndotl + 0.001); //·ÀÖ¹·ÖÄ¸Îª0

		L0 += radiance *  (flambert * kd + fcooktorrance * ks) * ndotl;
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + L0;
	color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 
	FragColor = vec4(color, 1.0);
}
