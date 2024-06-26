#version 330 core

in vec3 sPosition;
in vec2 sTexCoord;
in vec3 sNormal;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalTexture;

void main() {
	outPosition = sPosition;
	outNormal = normalize(texture(uNormalTexture, sTexCoord).rgb);
	outAlbedoSpec.rgb = texture(uDiffuseTexture, sTexCoord).rgb;
	outAlbedoSpec.a = 0.0;
}