#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 worldNormal;
} fs_in;
/* Unity的Maskmap
* Red: Metallic
* Green: Occlusion
* Blue: DetailMask
* Alpha: Smoothness
*/

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_maskmap;
uniform sampler2D texture_AO1;

uniform samplerCube irradianceMap;

const float PI = 3.14159265359;
vec3 Fresnel(float costheta, vec3 F0){
	//costheta = hdotv
	return F0 + (1.0 - F0) * pow(1.0 - costheta, 5.0);
}

float GGX(float costheta, float roughness){
	//costheta = ndotv
	// float k = (roughness + 1) * (roughness + 1) / 8;
	float k = roughness * roughness / 2;
	return costheta / (costheta * (1 - k) + k);
}

float NDF(float roughness, float costheta){ 
	//costheta = ndoth
	float division = costheta * costheta * (roughness * roughness - 1) + 1;
	division = division * division;
	return roughness * roughness / (PI * division); 
}

void main(){
    vec2 uv = fs_in.TexCoord;
    vec3 albedo = texture(texture_diffuse1, uv).xyz;
	vec4 maskMap_Color = texture(texture_maskmap, uv);
	//Get TangentSpace Normal
	vec3 n = texture(texture_normal1, uv).xyz; // sampling normal, TangentSpace, [0, 1]
	n = normalize(n * 2.0 - 1.0);//TangentSpace, [-1, 1]
	vec3 v = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 l = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	
	vec3 ao_color = texture(texture_AO1, uv).xyz;
	vec3 irradiance = texture(irradianceMap, fs_in.worldNormal).rgb;
	
	float roughness = 0.35;
	float metallic = maskMap_Color.r;
	float ao = ao_color.r;
    vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	vec3 lightColor = vec3(300.0, 300.0, 300.0);
	
	vec3 L0 = vec3(0.0);

	float ndotv = max(dot(n, v), 0.0);
	vec3 flambert = albedo / PI;
	
	vec3 h = normalize(v + l);
	float ndoth = max(dot(n, h), 0.0);
	float hdotv = max(dot(h, v), 0.0);
	float ndotl = max(dot(n, l), 0.0);
	float distance = length(fs_in.TangentFragPos - fs_in.TangentLightPos);
	float attenuation = 1 / (distance * distance);
	vec3 radiance = lightColor * attenuation;

	vec3 ks = Fresnel(hdotv, F0);
	//vec3 kd = (1.0 - metallic) * (vec3(1.0) - ks);
	vec3 kd = 1.0 - ks;
	//教程中kd = 1.0 - ks
	float fcooktorrance = GGX(ndotv, roughness) * NDF(roughness, ndoth) / (4.0 * ndotv * ndotl + 0.001); 

	L0 += radiance *  (flambert * kd + fcooktorrance * ks) * ndotl;

	//vec3 ambient = vec3(0.03) * albedo * ao; //原直接光
	//vec3 ambient = (kd * irradiance) * ao;
	vec3 ambient = irradiance;
	//vec3 color = ambient + L0;
	vec3 color = ambient;
	
	//color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0);
}
