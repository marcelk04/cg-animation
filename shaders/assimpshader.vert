#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in ivec4 aBoneIDs;
layout(location = 5) in vec4 aWeights;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;
uniform mat4 boneMatrices[100];  // Adjust size according to your needs

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 Tangent;

void main() {
    mat4 boneTransform = aWeights[0] * boneMatrices[aBoneIDs[0]] +
    aWeights[1] * boneMatrices[aBoneIDs[1]] +
    aWeights[2] * boneMatrices[aBoneIDs[2]] +
    aWeights[3] * boneMatrices[aBoneIDs[3]];

    //vec4 transformedPos = boneTransform * vec4(aPos, 1.0);
    vec4 transformedPos = vec4(aPos, 1.0);
    FragPos = vec3(model * transformedPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Tangent = mat3(transpose(inverse(model))) * aTangent;
    TexCoord = aTexCoord;

    gl_Position = projection * view * model * transformedPos;
}
