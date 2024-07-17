#version 330 core

in vec3 v_Position;
in float v_Lifetime;
in float v_Type;
vec4 u_StartColor;
vec4 u_EndColor;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;


void main()
{
//    if (v_Type > 0.5){
//        u_StartColor = vec4(0.5, 0.5, 0.5, 1.0); // Grey
//        u_EndColor = vec4(0.2, 0.2, 0.2, 1.0);   // Dark Grey
//    }
//    else {
//        u_StartColor = vec4(1.0, 0.5, 0.0, 1.0); // Orange
//        u_EndColor = vec4(1.0, 0.0, 0.0, 1.0);   // Red
//    }
    u_StartColor = vec4(2.0, 1.0, 0.0, 1.0); // Orange
    u_EndColor = vec4(2.0, 0.0, 0.0, 1.0);   // Red

    //o_Color = mix(u_EndColor, u_StartColor, v_Lifetime);

    outPosition = v_Position;
    outNormal = vec3(1.0);
    outAlbedoSpec.rgb = mix(u_EndColor, u_StartColor, v_Lifetime).rgb;
    outAlbedoSpec.a = 0.0;
}
