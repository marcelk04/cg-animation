#version 330 core

#define NR_POINT_LIGHTS 5

struct DirLight {
	vec3 direction; // points toward light source, normalized
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
uniform sampler2D uShadowMap;

uniform DirLight uDirLight;
uniform PointLight uPointLights[NR_POINT_LIGHTS];
uniform vec3 uCamPos;
uniform mat4 uLightSpaceMatrix;
uniform bool uEnableShadows;

float shadowCalculation(vec4 lightSpaceFragPos, float bias) {
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;

	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0) {
		return 0.0;
	}

	float currentDepth = projCoords.z;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	return shadow / 9.0;
}

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
		light.direction,
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

	vec4 lightSpaceFragPos = uLightSpaceMatrix * vec4(fragPos, 1.0);
	vec3 viewDir = normalize(uCamPos - fragPos);

	vec3 result = vec3(0.0);

	// add directional light
	result += calcDirLight(uDirLight, viewDir, normal, albedo, specular);

	// add point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		float dist = length(uPointLights[i].position - fragPos);

		if (dist < uPointLights[i].radius) {
			result += calcPointLight(uPointLights[i], viewDir, normal, fragPos, albedo, specular, dist);
		}
	}

	// shadow casting
	float shadow = 0.0;

	if (uEnableShadows) {
		float bias = max(0.05 * (1.0 - dot(normal, uDirLight.direction)), 0.005);

		shadow = shadowCalculation(lightSpaceFragPos, bias);
	}

	result = (1 - shadow) * result;

	// ambient lighting
	result += albedo * 0.1;

	outColor = vec4(result, 1.0);

	// extract bright areas for bloom effect
	float luminance = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (luminance > 1.0) {
		outBrightColor = vec4(outColor.rgb, 1.0);
	} else {
		outBrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}