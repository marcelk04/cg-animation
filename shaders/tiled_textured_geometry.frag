#version 330 core

in vec3 sPosition;
in vec2 sTexCoord;
in vec3 sNormal;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform sampler2D uDiffuseTexture;
uniform float uTileFactor;

void main() {
	outPosition = sPosition;
	outNormal = normalize(sNormal);
	vec2 texCoord = sTexCoord * uTileFactor - floor(sTexCoord * uTileFactor);
	outAlbedoSpec.rgb = texture(uDiffuseTexture, texCoord).rgb;
	outAlbedoSpec.a = 0.0;
}