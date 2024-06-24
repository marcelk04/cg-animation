#version 330 core

struct DirLight {
	vec3 direction; // points toward light source
	vec3 color;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float radius;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 4

in vec2 sTexCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outBrightColor;

uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedoSpec;

uniform DirLight uDirLight;
uniform PointLight uPointLights[NR_POINT_LIGHTS];
uniform vec3 uCamPos;

vec3 calcDirLight(DirLight light, vec3 albedo, float specular, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.direction);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo * light.color;

	float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
	vec3 specularRes = light.color * spec * specular;

	return diffuse + specularRes;
}

vec3 calcPointLight(PointLight light, vec3 fragPos, vec3 albedo, float specular, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo * light.color;

	float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
	vec3 specularRes = light.color * spec * specular;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	diffuse *= attenuation;
	specularRes *= attenuation;

	return diffuse + specularRes;
}

void main() {
	vec3 fragPos = texture(uPosition, sTexCoord).xyz;
	vec3 normal = texture(uNormal, sTexCoord).xyz;
	vec3 albedo = texture(uAlbedoSpec, sTexCoord).rbg;
	float specular = texture(uAlbedoSpec, sTexCoord).a;

	vec3 viewDir = normalize(uCamPos - fragPos);

	vec3 result = albedo * 0.1;

	result += calcDirLight(uDirLight, albedo, specular, normal, viewDir);

	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		float dist = length(uPointLights[i].position - fragPos);

		if (dist < uPointLights[i].radius) {
			result += calcPointLight(uPointLights[i], fragPos, albedo, specular, normal, viewDir);
		}
	}

	outColor = vec4(result, 1.0);

	float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		outBrightColor = vec4(outColor.rgb, 1.0);
	} else {
		outBrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}