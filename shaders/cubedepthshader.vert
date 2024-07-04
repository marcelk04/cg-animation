#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 uLocalToWorld;

void main() {
	gl_Position = uLocalToWorld * vec4(inPosition, 1.0);
}