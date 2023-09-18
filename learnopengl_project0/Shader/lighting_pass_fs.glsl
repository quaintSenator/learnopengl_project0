#version 330 core
out vec4 FragColor;

in vec2 quad_uv;

uniform sampler2D G_FragPos;
uniform sampler2D G_Normal;
uniform sampler2D G_Albedo;
uniform sampler2D G_AO;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 lightPos[100];
uniform vec3 lightColor[100];
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

void main()
{    
    //vec2 uv = texture(G_uv, quad_uv).xy;
    vec2 uv= quad_uv;
    
    vec3 albedo = texture(G_Albedo, uv).xyz;
	float metallic = texture(G_AO, uv).g;
	float ao = texture(G_AO, uv).r;
    vec3 n = texture(G_Normal, uv).xyz;//法线贴图精法线, 世界空间
    //n = normalize(n * 2.0 - 1.0);//TangentSpace, [-1, 1]
    vec3 N = texture(G_Normal, uv).rgb;//Gbuffer 粗法线， 世界空间
    //mat3 TBN = mat3(Tangent, Bitangent, N);
    
    //n = normalize(TBN * n);//法线贴图精法线， 世界空间
    
    vec3 fragPos = texture(G_FragPos, uv).rgb;
    vec3 V = normalize(cameraPos - fragPos);//世界空间观察方向
    vec3 R = reflect(-V, N);
	
	float roughness = 0.35;
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	vec3 L0 = vec3(0.0);

	float ndotv = max(dot(n, V), 0.0);
	vec3 flambert = albedo / PI;
	
	// ----------------------------------------------------------------------------
    //直接光部分
    for(int i = 0; i < 100; i++)
    {
        vec3 l = normalize(lightPos[i] - fragPos);//世界空间光照方向
        vec3 h = normalize(V + l);
        float ndoth = max(dot(n, h), 0.0);
	    float hdotv = max(dot(h, V), 0.0);
	    float ndotl = max(dot(n, l), 0.0);
	    float distance = length(fragPos - lightPos[i]);
	    //float attenuation = distance < 5.0 ? 1/125.0 : 1 / pow(distance, 3);
	    float attenuation = 1 / distance;
	    vec3 directLightRadiance = lightColor[i];
	    //vec3 ks = Fresnel(hdotv, F0);
	    vec3 ks = vec3(pow(1.0 - hdotv, 5.0));
	    vec3 kd = 1.0 - ks;
	    float NDF = DistributionGGX(n, h, roughness);
	    float G = GeometrySmith(n, V, l, roughness);
	    vec3 numerator  = NDF * G * ks;
	    float denominator = 4.0 * max(dot(n, V), 0.0) * max(dot(n, l), 0.0) + 0.0001;
	    vec3 specular = numerator / denominator;
	    L0 += directLightRadiance * (kd * albedo / PI + specular) * ndotl; 
    }
    // ----------------------------------------------------------------------------
    //环境光部分
    
     //采样漫反射贴图
    vec3 irradiance = texture(irradianceMap, n).rgb;
    vec3 diffuseColor = irradiance * albedo;
    /*
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_maskmap;
uniform sampler2D texture_AO1;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 cameraPos;
uniform vec3 lightPos[5];
*/
    //采样lut和预滤波贴图
    const float MAX_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(ndotv, roughness)).rg;
    
    //计算specular
    vec3 specular = prefilteredColor * (F0 * brdf.x + brdf.y);
    vec3 F = FresnelRoughness(ndotv, F0, roughness);
    vec3 kd = 1.0 - F;
	vec3 ambient =  (kd * diffuseColor + specular) * ao;
	vec3 color = ambient + L0;
	
	FragColor = vec4(L0, 1.0);
}