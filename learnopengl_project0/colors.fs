#version 330 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light{
	vec3 lightPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//uniform vec3 lightColor;
//uniform vec3 lightPos;
//uniform vec3 objectColor;
uniform vec3 viewPos;

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

void main(){
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.lightPos - FragPos);
	float diffCos = max(0.0, dot(lightDir,norm));
	vec3 diffuseColor = diffCos * light.diffuse * material.diffuse;

	vec3 ambientColor = material.ambient * light.ambient;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specCos = dot(norm, halfDir);
	vec3 specularColor = light.specular * material.specular * pow(specCos, material.shininess);

	FragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
	
}