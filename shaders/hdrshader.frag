#version 330 core

in vec2 sTexCoord;

out vec4 outColor;

uniform sampler2D uHdrBuffer;

void main() {
	vec3 hdrColor = texture(uHdrBuffer, sTexCoord).rgb;
	outColor = vec4(hdrColor, 1.0);
}