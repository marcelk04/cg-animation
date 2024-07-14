#version 330 core

in vec2 sTexCoord;

out vec4 outColor;

uniform sampler2D uHdrBuffer;
uniform sampler2D uBloomBuffer;
uniform float uExposure = 1.0;
uniform float uGamma = 2.2;

void main() {
	vec3 hdrColor = texture(uHdrBuffer, sTexCoord).rgb;
	vec3 bloomColor = texture(uBloomBuffer, sTexCoord).rbg;

	hdrColor += bloomColor; // additive blending

	// exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * uExposure);

	// gamma correction
	mapped = pow(mapped, vec3(1.0 / uGamma));

	outColor = vec4(mapped, 1.0);
}