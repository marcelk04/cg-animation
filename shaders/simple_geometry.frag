#version 330 core

struct Material {
	vec3 diffuse;
	float specular;
};

in vec3 sPosition;
in vec2 sTexCoord;
in vec3 sNormal;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform Material uMaterial;

void main() {
	outPosition = sPosition;
	outNormal = normalize(sNormal);
	outAlbedoSpec.rgb = uMaterial.diffuse;
	outAlbedoSpec.a = uMaterial.specular;
}