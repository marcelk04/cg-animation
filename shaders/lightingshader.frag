#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirLight {
	vec3 direction; // points toward light source

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 4

in vec3 sNormal;
in vec3 sFragPos;

out vec4 outColor;

uniform Material uMaterial;
uniform DirLight uDirLight;
uniform PointLight uPointLights[NR_POINT_LIGHTS];
uniform vec3 uCamPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = light.ambient * uMaterial.ambient;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * uMaterial.diffuse);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
	vec3 specular = light.specular * (spec * uMaterial.specular);

	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - sFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = light.ambient * uMaterial.ambient;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * uMaterial.diffuse);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
	vec3 specular = light.specular * (spec * uMaterial.specular);

	float distance = length(light.position - sFragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

void main() {
	vec3 normal = normalize(sNormal);
	vec3 viewDir = normalize(uCamPos - sFragPos);

	vec3 result = calcDirLight(uDirLight, normal, viewDir);

	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		result += calcPointLight(uPointLights[i], normal, viewDir);
	}

	outColor = vec4(result, 1.0);
}