#version 330 core

in vec3 sPosition;
in vec2 sTexCoord;
in vec3 sNormal;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

//uniform sampler2D texture_diffuse1;

void main() {
    //vec3 color = texture(texture_diffuse1, TexCoord).rgb;
    
    outPosition = sPosition;
    outNormal = normalize(sNormal);
    outAlbedoSpec = vec4(1.0);
}
