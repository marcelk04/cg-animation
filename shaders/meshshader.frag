#version 330 core

in vec3 sPosition;
in vec3 sNormal;

out vec3 outColor;

uniform vec3 uLightDir = normalize(vec3(1.0));

void main() {
	vec3 normal = normalize(sNormal);
	vec3 color = vec3(1.0, 1.0, 1.0);

	outColor = color * max(dot(uLightDir, normal), 0.0);
}