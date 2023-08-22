#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 i_normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

vec3 m_reflect(vec3 lightDir,vec3 normal)//both params are normalized
{   
    float cos = dot(lightDir,normal);
    vec3 light_proj2_normal = normal * cos;
    vec3 light_proj2_tangant = lightDir - light_proj2_normal;
    
    vec3 result = -light_proj2_tangant + light_proj2_normal;
    return result;//result也是normalized
}
void main()
{
    /*vec3 frag_normal = normalize(i_normal);
    vec3 lightDir = normalize(lightPos - FragPos.xyz);
    vec3 eyeDir = cameraPos - FragPos.xyz;
    eyeDir = normalize(eyeDir);
    
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * lightColor;
    
    float diffuse_term = dot(frag_normal,lightDir);
    diffuse_term = diffuse_term < 0.0 ? 0.0 : diffuse_term;
    vec3 diffuseColor = diffuse_term * lightColor;
    
    float specularStrength = 0.5;
    float shininess = 32;
    //vec3 reflectionDir = reflect(-lightDir, frag_normal);
    vec3 reflectionDir = m_reflect(lightDir, frag_normal);
    
    float cos_viewDir_reflectDir = 0.0;
    if(dot(frag_normal, lightDir) < 0.0)//背面光，剔除
    {
        cos_viewDir_reflectDir = 0.0;
    }
    else
    {
        cos_viewDir_reflectDir = dot(reflectionDir,eyeDir);
        cos_viewDir_reflectDir = cos_viewDir_reflectDir < 0.0 ? 0.0 : cos_viewDir_reflectDir;
    }
    float specular_term = pow(cos_viewDir_reflectDir,shininess);
    vec3 specularColor = specularStrength * specular_term * lightColor;
    
    vec3 calculatedColor = ambientColor + diffuseColor + specularColor;
    FragColor = vec4(calculatedColor * objectColor, 1.0);*/
    
    //示例代码 显示结果却有点问题
    // 用-lightDir求反射
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(i_normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

