#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 sNormal;
in vec3 sFragPos;

out vec4 outColor;

uniform Material uMaterial;
uniform Light uLight;
uniform vec3 uCamPos;

void main() {
	vec3 normal = normalize(sNormal);
	vec3 lightDir = normalize(uLight.position - sFragPos);
	vec3 viewDir = normalize(uCamPos - sFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	// ambient
	vec3 ambient = uLight.ambient * uMaterial.ambient;

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

	// specular
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
	vec3 specular = uLight.specular * (spec * uMaterial.specular);

	vec3 result = ambient + diffuse + specular;
	outColor = vec4(result, 1.0);
}