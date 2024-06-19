#version 330 core

in vec3 sNormal;
in vec3 sFragPos;

out vec4 outColor;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uCamPos;

void main() {
	vec3 normal = normalize(sNormal);
	vec3 lightDir = normalize(uLightPos - sFragPos);
	
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * uLightColor;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * uLightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(uCamPos - sFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * uLightColor;

	vec3 result = (ambient + diffuse + specular) * uObjectColor;
	
	outColor = vec4(result, 1.0);
}