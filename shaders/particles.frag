#version 450 core

layout (location = 0) out vec4 o_Color;

in float v_Lifetime;

uniform vec4 u_StartColor;
uniform vec4 u_EndColor;

void main()
{
    o_Color = mix(u_EndColor, u_StartColor, v_Lifetime);
}
