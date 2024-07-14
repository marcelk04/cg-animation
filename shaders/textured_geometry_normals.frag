#version 330 core

in vec3 sPosition;
in vec2 sTexCoord;
in vec3 sNormal;
in mat3 sTBN;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalTexture;

void main() {
	outPosition = sPosition;
	outNormal = texture(uNormalTexture, sTexCoord).rgb;
	outNormal = outNormal * 2.0 - 1.0;
	outNormal = normalize(sTBN * outNormal);
	outAlbedoSpec.rgb = texture(uDiffuseTexture, sTexCoord).rgb;
	outAlbedoSpec.a = 0.0;
}