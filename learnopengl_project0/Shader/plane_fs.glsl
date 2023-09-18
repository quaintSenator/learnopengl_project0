#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 uv;
    vec4 FragPosInLightSpace[5];
} fs_in;

uniform sampler2D planeTexture;
uniform sampler2D shadowMap[5];
uniform vec3 lightPos[5];
uniform vec3 viewPos;

float calculate_shadow(int i)
{
    //FragPosInLightSpace的深度范围是NDC [-1,1], 且没有进行过透视除法
    //渲染管线为gl_Position默认进行透视除法，而FragPosInLightSpace是我们自己乘的透视矩阵，因此没透除
    vec3 FragPosAfterProspectiveDivision = fs_in.FragPosInLightSpace[i].xyz / fs_in.FragPosInLightSpace[i].w;
    vec3 FragPosLinear = (FragPosAfterProspectiveDivision + 1) / 2;
    float FragDepth = FragPosLinear.z;
    float shadowMapDepth = texture(shadowMap[i], FragPosLinear.xy).r;
    float shadow = FragDepth - shadowMapDepth > 0.005 ? 1.0 : 0.0;
    if(FragDepth > 0.99f)
    {
        shadow = -1.0f;
    }
    return shadow;
}
void main()
{    
    vec3 textureColor = texture(planeTexture, fs_in.uv).xyz;
    float shadow = 0.0;
    float diffuse = 0.0;
    float spec = 0.0;
    for(int i = 0; i <= 4; i++)
    {
        float curShadow = calculate_shadow(i);
        if(curShadow > 0.0)
            shadow += calculate_shadow(i);
        vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        diffuse += max(0.0, dot(lightDir, fs_in.Normal));
        vec3 halfway = normalize(lightDir + viewDir);
        float powDot = max(dot(halfway, fs_in.Normal), 0.0);
        spec += pow(powDot, 64);
    }
    vec3 diffuseColor = textureColor * clamp(diffuse, 0.0, 1.0);
    vec3 specularColor = vec3(0.3) * clamp(spec, 0.0, 1.0);
    vec3 ambient = 0.1 * textureColor;
    shadow = clamp(shadow, 0.0, 1.0);
    //float distance = length(fs_in.FragPos - lightPos);
	//float attenuation = 1 / (distance * distance);
    vec3 resultColor = ambient + (1.0 - shadow) * (diffuseColor + specularColor);
    //resultColor *= attenuation;
    FragColor = vec4(resultColor, 1.0);
}