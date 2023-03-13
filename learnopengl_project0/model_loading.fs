#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 pointPos;
in vec3 pointNormal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 model;

void main()
{    
    vec3 DiffuseColor = texture(texture_diffuse1, TexCoords).xyz;
    vec3 SpecularColor = texture(texture_specular1, TexCoords).xyz;
    vec3 AmbientColor = texture(texture_diffuse1, TexCoords).xyz * 0.4;
    //vec3 SpecularColor = vec3(0.0, 0.0, 0.0);
    vec3 rl = (model * vec4(lightPos, 1.0)).xyz;
    vec3 rv = (model * vec4(viewPos, 1.0)).xyz;

    vec3 l = normalize(rl - pointPos);
    vec3 n = normalize(pointNormal);
    vec3 e = normalize(rv - pointPos);
    float shininess = 3.0;
    vec3 h = normalize(e + l);
    float hdotn = max(dot(h,n), 0.0);
    float ldotn = max(dot(l,n), 0.0);

    vec3 result = DiffuseColor * hdotn + SpecularColor * pow(hdotn, shininess);
    FragColor = vec4(result, 1.0);
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(1.0);
}