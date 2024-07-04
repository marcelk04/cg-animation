#version 330 core

in vec4 sFragPos;

uniform vec3 uLightPosition;
uniform float uFar;

void main() {
	float distanceToLight = length(sFragPos.xyz - uLightPosition);

	// map to range [0, 1]
	distanceToLight = distanceToLight / uFar;

	gl_FragDepth = distanceToLight;
}