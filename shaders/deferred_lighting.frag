#version 330 core

#define NR_POINT_LIGHTS 5

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

in vec2 sTexCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outBrightColor;

// textures from geometry pass
uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedoSpec;

uniform DirLight uDirLight;
uniform PointLight uPointLights[NR_POINT_LIGHTS];
uniform vec3 uCamPos;

vec3 calcLight(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 albedo, float specular, vec3 lightColor) {
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float diffuseVal = max(dot(normal, lightDir), 0.0);
	vec3 diffuseResult = diffuseVal * albedo * lightColor;

	float specularVal = pow(max(dot(normal, halfwayDir), 0.0), 32);
	vec3 specularResult = specularVal * specular * lightColor;

	return diffuseResult + specularResult;
}

vec3 calcDirLight(DirLight light, vec3 viewDir, vec3 normal, vec3 albedo, float specular) {
	return calcLight(
		normalize(light.direction),
		viewDir,
		normal,
		albedo,
		specular,
		light.color);
}

vec3 calcPointLight(PointLight light, vec3 viewDir, vec3 normal, vec3 fragPos, vec3 albedo, float specular, float dist) {
	vec3 lightResult = calcLight(
		normalize(light.position - fragPos),
		viewDir,
		normal,
		albedo,
		specular,
		light.color);

	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

	return attenuation * lightResult;
}

void main() {
	vec3 fragPos = texture(uPosition, sTexCoord).xyz;
	vec3 normal = texture(uNormal, sTexCoord).xyz;
	vec3 albedo = texture(uAlbedoSpec, sTexCoord).rbg;
	float specular = texture(uAlbedoSpec, sTexCoord).a;

	vec3 viewDir = normalize(uCamPos - fragPos);

	// ambient lighting
	vec3 result = albedo * 0.1;

	result += calcDirLight(uDirLight, viewDir, normal, albedo, specular);

	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		float dist = length(uPointLights[i].position - fragPos);

		if (dist < uPointLights[i].radius) {
			result += calcPointLight(uPointLights[i], viewDir, normal, fragPos, albedo, specular, dist);
		}
	}

	outColor = vec4(result, 1.0);

	float luminance = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (luminance > 1.0) {
		outBrightColor = vec4(outColor.rgb, 1.0);
	} else {
		outBrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}