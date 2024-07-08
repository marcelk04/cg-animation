#version 330 core

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outBrightColor;

uniform vec3 uObjectColor;

void main() {
	outColor = vec4(uObjectColor, 1.0);

	float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		outBrightColor = vec4(outColor.rgb, 1.0);
	} else {
		outBrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}