#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 uv; 
    vec4 FragPosInLightSpace;
} fs_in;

uniform sampler2D planeTexture;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;


float calculate_shadow(vec4 FragPosInLightSpace)
{
    //FragPosInLightSpace的深度范围是NDC [-1,1], 且没有进行过透视除法
    //渲染管线为gl_Position默认进行透视除法，而FragPosInLightSpace是我们自己乘的透视矩阵，因此没透除
    vec3 FragPosAfterProspectiveDivision = FragPosInLightSpace.xyz / FragPosInLightSpace.w;
    vec3 FragPosLinear = (FragPosAfterProspectiveDivision + 1) / 2;
    float FragDepth = FragPosLinear.z;
    
    float shadowMapDepth = texture(shadowMap, FragPosLinear.xy).r;
    float shadow = FragDepth - shadowMapDepth > 0.005 ? 1.0 : 0.0;
    
    if(FragDepth > 1.0)
    {
        shadow = 0.0f;
    }
    return shadow;
}
void main()
{    
    vec3 textureColor = texture(planeTexture, fs_in.uv).xyz;
    
    float shadow = calculate_shadow(fs_in.FragPosInLightSpace);
    vec3 ambient = 0.1 * textureColor;
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 diffuse = textureColor * max(0.0, dot(lightDir, fs_in.Normal));
    
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = max(dot(halfway, fs_in.Normal), 0.0);
    spec = pow(spec, 64);
    
    vec3 specular = spec * vec3(0.3);
    
    //float distance = length(fs_in.FragPos - lightPos);
	//float attenuation = 1 / (distance * distance);
	
    vec3 resultColor = ambient + (1.0 - shadow) * (diffuse + specular);
    //resultColor *= attenuation;
    FragColor = vec4(resultColor, 1.0);
}