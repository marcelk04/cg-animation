#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

uniform mat4 uWorldToClip = mat4(1.0);
uniform mat4 uLocalToWorld = mat4(1.0);

void main() {
	gl_Position = uWorldToClip * uLocalToWorld * vec4(inPosition, 1.0);
}