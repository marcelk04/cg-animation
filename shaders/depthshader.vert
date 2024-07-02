#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uLocalToWorld;

void main () {
	gl_Position = uLightSpaceMatrix * uLocalToWorld * vec4(inPosition, 1.0);
}