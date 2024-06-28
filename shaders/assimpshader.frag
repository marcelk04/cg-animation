#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;

uniform sampler2D texture_diffuse1;

void main() {
    vec3 color = texture(texture_diffuse1, TexCoord).rgb;
    FragColor = vec4(color, 1.0);
}
