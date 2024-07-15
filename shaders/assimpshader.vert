#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in ivec4 inBoneIds;
layout(location = 5) in vec4 inWeights;

out vec3 sPosition;
out vec2 sTexCoord;
out vec3 sNormal;

uniform mat4 uWorldToClip = mat4(1.0);
uniform mat4 uLocalToWorld = mat4(1.0);
uniform mat3 uNormalMatrix = mat3(1.0);

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 uFinalBonesMatrices[MAX_BONES];

void main()
{
    /*
    vec4 totalPosition = vec4(0.0);
    //mat4 totalTransform = mat4(0.0);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(inBoneIds[i] == -1)
        {
            continue;
        }

        if(inBoneIds[i] >= MAX_BONES)
        {
            totalPosition = vec4(inPosition,1.0);
            //totalTransform = mat4(1.0);
            break;
        }

        //totalTransform += finalBonesMatrices[boneIds[i]] * weights[i];

        vec4 localPosition = uFinalBonesMatrices[inBoneIds[i]] * vec4(inPosition, 1.0);
        totalPosition += localPosition * inWeights[i];
        //vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
    }
    
    gl_Position = uWorldToClip * uLocalToWorld * totalPosition;
    
    sPosition = vec3(uLocalToWorld * totalPosition);
    sTexCoord = inTexCoord;
    sNormal = uNormalMatrix * inNormal;
    */

    mat4 boneTransform = uFinalBonesMatrices[inBoneIds[0]] * inWeights[0]
        + uFinalBonesMatrices[inBoneIds[1]] * inWeights[1]
        + uFinalBonesMatrices[inBoneIds[2]] * inWeights[2]
        + uFinalBonesMatrices[inBoneIds[3]] * inWeights[3];

    vec4 worldPosition = uLocalToWorld * boneTransform * vec4(inPosition, 1.0);

    gl_Position = uWorldToClip * worldPosition;

    sPosition = worldPosition.xyz;
    sTexCoord = inTexCoord;
    sNormal = uNormalMatrix * transpose(inverse(mat3(boneTransform))) * inNormal;
}