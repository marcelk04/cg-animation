#version 330 core

out vec4 outColor;

uniform vec3 uObjectColor;

void main() {
	outColor = vec4(uObjectColor, 1.0);
}