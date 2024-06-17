#version 330 core

in vec3 sPosition;
in vec2 sTexCoord;
in vec3 sNormal;

out vec3 outColor;

uniform sampler2D uTexture;

uniform vec3 uLightDir = normalize(vec3(1.0));

void main() {
	vec4 color = texture(uTexture, sTexCoord);

	if (color.a < 0.1) {
		discard;
	}

	outColor = color.rgb;
	//outColor = vec3(sTexCoord, 0.0);
}