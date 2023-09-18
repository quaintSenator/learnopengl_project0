#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 uv;
} fs_in;

uniform sampler2D planeTexture;
uniform vec3 lightPos[100];
uniform vec3 lightColor[100];
uniform vec3 viewPos;

void main()
{    
    vec3 textureColor = texture(planeTexture, fs_in.uv).xyz;
    vec3 diffuseColor = vec3(0.0);
    vec3 specularColor = vec3(0.0);
    for(int i = 0; i < 100; i++)
    {
        float distance2light = length(lightPos[i] - fs_in.FragPos);
        float attenuation = 1 / pow(distance2light, 3);
        vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 halfway = normalize(lightDir + viewDir);
        diffuseColor += lightColor[i] * max(0.0, dot(lightDir, fs_in.Normal)) / 100;
        float powDot = max(dot(halfway, fs_in.Normal), 0.0);
        specularColor += lightColor[i] * pow(powDot, 256);
    }
    diffuseColor = textureColor * diffuseColor;
    vec3 ambient = 0.1 * textureColor;
    //float distance = length(fs_in.FragPos - lightPos);
	//float attenuation = 1 / (distance * distance);
    vec3 resultColor = ambient + diffuseColor + specularColor;
    //resultColor *= attenuation;
    FragColor = vec4(resultColor, 1.0);
}