#version 330 core

in vec2 sTexCoord;

out vec4 outColor;

uniform sampler2D uColorBuffer;

uniform bool uHorizontal;
uniform float uOffsets[3] = float[] (0.0, 1.384615, 3.230769);
uniform float uWeights[3] = float[] (0.227027, 0.316216, 0.070270);

void main() {
	vec2 texOffset = 1.0 / textureSize(uColorBuffer, 0); // gets size of a single texel
	vec3 result = vec3(0.0);

	if (uHorizontal) {
		for (int i = 0; i < 3; i++) {
			result += texture(uColorBuffer, sTexCoord + vec2(texOffset.x * i, 0.0)).rgb * uWeights[i];
			result += texture(uColorBuffer, sTexCoord - vec2(texOffset.x * i, 0.0)).rgb * uWeights[i];
		}
	} else {
		for (int i = 0; i < 3; i++) {
			result += texture(uColorBuffer, sTexCoord + vec2(0.0, texOffset.y * i)).rgb * uWeights[i];
			result += texture(uColorBuffer, sTexCoord - vec2(0.0, texOffset.y * i)).rgb * uWeights[i];
		}
	}

	outColor = vec4(result, 1.0);
}