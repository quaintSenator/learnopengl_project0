#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light{
	vec3 lightPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

void main(){
	vec3 diffusemap = texture(material.diffuse, TexCoord).rgb;
	vec3 specularMap = texture(material.specular, TexCoord).rgb;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.lightPos - FragPos);
	float diffCos = max(0.0, dot(lightDir,norm));
	vec3 diffuseColor = diffCos * (light.diffuse * diffusemap);

	vec3 ambientColor = diffusemap * light.ambient;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specCos = max(0.0,dot(norm, halfDir));
	vec3 specularColor = light.specular * (specularMap * pow(specCos, material.shininess));




	FragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
	
	
	
}