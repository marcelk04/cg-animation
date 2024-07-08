#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Velocity;
layout (location = 2) in float a_Lifetime;
layout (location = 3) in float a_Type;

uniform mat4 u_ViewProj;
uniform float u_Time;

out vec3 v_Position;
out float v_Type;
out float v_Lifetime;

void main()
{
    v_Position;
    float age = mod(u_Time, a_Lifetime);
    vec3 currentPosition = a_Position + a_Velocity * age;
    gl_Position = u_ViewProj * vec4(currentPosition, 1.0);
    v_Lifetime = 1.0 - (age / a_Lifetime);
    v_Type = a_Type;
}
