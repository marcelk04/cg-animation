#version 330 core

out vec4 outColor;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;

void main() {
	outColor = vec4(uLightColor * uObjectColor, 1.0);
}