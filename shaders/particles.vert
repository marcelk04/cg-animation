#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Velocity;
layout (location = 2) in float a_Lifetime;

uniform mat4 u_ViewProj;
uniform float u_Time;

out float v_Lifetime;

void main()
{
    float age = mod(u_Time, a_Lifetime);
    vec3 currentPosition = a_Position + a_Velocity * age;
    gl_Position = u_ViewProj * vec4(currentPosition, 1.0);
    v_Lifetime = 1.0 - (age / a_Lifetime);
}
