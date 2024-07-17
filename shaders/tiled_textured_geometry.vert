#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

out vec3 sPosition;
out vec2 sTexCoord;
out vec3 sNormal;

uniform mat4 uWorldToClip = mat4(1.0);
uniform mat4 uLocalToWorld = mat4(1.0);
uniform mat3 uNormalMatrix = mat3(1.0);

void main() {
	gl_Position = uWorldToClip * uLocalToWorld * vec4(inPosition, 1.0);

	sPosition = vec3(uLocalToWorld * vec4(inPosition, 1.0));
	sTexCoord = inTexCoord;
	sNormal = uNormalMatrix * inNormal;
}