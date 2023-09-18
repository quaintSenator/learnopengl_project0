#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos[5];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 worldNormal;
    vec3 Normal;
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
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 cameraPos;

const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 Fresnel(float costheta, vec3 F0){
	//costheta = hdotv
	return F0 + (1.0 - F0) * pow(1.0 - costheta, 5.0);
}
vec3 FresnelRoughness(float costheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - costheta, 0.0, 1.0), 5.0);
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

	vec3 n = texture(texture_normal1, uv).xyz; // sampling normal, TangentSpace, [0, 1]
	n = normalize(n * 2.0 - 1.0);//TangentSpace, [-1, 1]
	vec3 v = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 N = fs_in.Normal;
	vec3 V = normalize(cameraPos - fs_in.FragPos);//世界坐标观察方向
	vec3 R = reflect(-V, N);
	vec3 lightColor = vec3(60.0, 60.0, 60.0);
	
	vec3 ao_color = texture(texture_AO1, uv).xyz;
	float ao = ao_color.r;
	float roughness = 0.35;
	float metallic = maskMap_Color.r;
	
    vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	vec3 L0 = vec3(0.0);

	float ndotv = max(dot(n, v), 0.0);
	vec3 flambert = albedo / PI;
    
// ----------------------------------------------------------------------------
    //直接光部分
	for(int i = 0; i < 5; i++)
	{
	    vec3 l = normalize(fs_in.TangentLightPos[i] - fs_in.TangentFragPos);
	    vec3 h = normalize(v + l);
	    float ndoth = max(dot(n, h), 0.0);
	    float hdotv = max(dot(h, v), 0.0);
	    float ndotl = max(dot(n, l), 0.0);
	    float distance = length(fs_in.TangentFragPos - fs_in.TangentLightPos[i]);
	    float attenuation = distance < 5.0 ? 1/125.0 : 1 / pow(distance, 3);
	    vec3 radiance = lightColor * attenuation;
	    vec3 ks = Fresnel(hdotv, F0);
	    //vec3 kd = (1.0 - metallic) * (vec3(1.0) - ks);
	    vec3 kd = 1.0 - ks;
	    //教程中kd = 1.0 - ks
	    //float fcooktorrance = GGX(ndotv, roughness) * NDF(roughness, ndoth) / (4.0 * ndotv * ndotl + 0.001); 
        //flambert为直接光版本的diffuse
        //现在应当改为irradiance版本
	    //L0 += radiance *  (flambert * kd + fcooktorrance * ks) * ndotl;
	    float NDF = DistributionGGX(n, h, roughness);
	    float G = GeometrySmith(n, v, l, roughness);
	    vec3 numerator    = NDF * G * ks;
        float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
	    L0 += radiance * (kd * albedo / PI + specular) * ndotl;  
	}
	// ----------------------------------------------------------------------------
    //环境光部分
    
    //采样漫反射贴图
    vec3 irradiance = texture(irradianceMap, fs_in.worldNormal).rgb;
    vec3 diffuceColor = irradiance * albedo;
    
    //采样lut和预滤波贴图
    const float MAX_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(ndotv, roughness)).rg;
    
    //计算specular
    vec3 specular = prefilteredColor * (F0 * brdf.x + brdf.y);
    vec3 F = FresnelRoughness(ndotv, F0, roughness);
    vec3 kd = 1.0 - F;
	vec3 ambient =  (kd * diffuceColor + specular) * ao;
	vec3 color = ambient + L0;
	//vec3 color = ambient;

	FragColor = vec4(color, 1.0);
}
