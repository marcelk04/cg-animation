#version 330 core

in vec2 sTexCoords;

out vec4 outColor;

uniform sampler2D uColorBuffer;

uniform bool uHorizontal;
uniform float uWeights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
	vec2 texOffset = 1.0 / textureSize(uColorBuffer, 0); // gets size of a single texelFetch
	vec3 result = texture(uColorBuffer, sTexCoords).rgb * uWeights[0];

	if (uHorizontal) {
		for (int i = 1; i < 5; i++) {
			result += texture(uColorBuffer, sTexCoords + vec2(texOffset.x * i, 0.0)).rgb * uWeights[i];
			result += texture(uColorBuffer, sTexCoords - vec2(texOffset.x * i, 0.0)).rgb * uWeights[i];
		}
	} else {
		for (int i = 1; i < 5; i++) {
			result += texture(uColorBuffer, sTexCoords + vec2(0.0, texOffset.y * i)).rgb * uWeights[i];
			result += texture(uColorBuffer, sTexCoords - vec2(0.0, texOffset.y * i)).rgb * uWeights[i];
		}
	}

	outColor = vec4(result, 1.0);
}