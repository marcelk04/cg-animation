#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

void main() {
    //vec3 color = texture(texture_diffuse1, TexCoord).rgb;
    vec3 color = vec3(1.0);
    FragColor = vec4(color, 1.0);
}
