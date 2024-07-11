#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "animator.hpp"

Animator::Animator(const aiAnimation* animation, const aiNode* rootNode)
        : m_CurrentAnimation(animation), m_RootNode(rootNode), m_CurrentTime(0.0f), m_BoneCount(0) {
    m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
}

void Animator::UpdateAnimation(float dt) {
    m_CurrentTime += m_CurrentAnimation->mTicksPerSecond * dt;
    float animationTime = fmod(m_CurrentTime, m_CurrentAnimation->mDuration);
    CalculateBoneTransform(m_RootNode, glm::mat4(1.0f), animationTime);
}

void Animator::CalculateBoneTransform(const aiNode* node, glm::mat4 parentTransform, float animationTime) {
    std::string nodeName(node->mName.data);
    glm::mat4 nodeTransform = ConvertMatrixToGLMFormat(node->mTransformation);

    const aiNodeAnim* nodeAnim = FindNodeAnim(m_CurrentAnimation, nodeName);

    if (nodeAnim) {
        glm::vec3 scaling = InterpolateScaling(animationTime, nodeAnim);
        glm::mat4 scalingM = glm::scale(glm::mat4(1.0f), scaling);

        glm::quat rotationQ = InterpolateRotation(animationTime, nodeAnim);
        glm::mat4 rotationM = glm::toMat4(rotationQ);

        glm::vec3 translation = InterpolatePosition(animationTime, nodeAnim);
        glm::mat4 translationM = glm::translate(glm::mat4(1.0f), translation);

        nodeTransform = translationM * rotationM * scalingM;
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    if (m_BoneInfoMap.find(nodeName) != m_BoneInfoMap.end()) {
        m_BoneInfoMap[nodeName].FinalTransformation = globalTransformation * m_BoneInfoMap[nodeName].BoneOffset;
        m_FinalBoneMatrices[m_BoneInfoMap[nodeName].id] = m_BoneInfoMap[nodeName].FinalTransformation;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        CalculateBoneTransform(node->mChildren[i], globalTransformation, animationTime);
    }
}

glm::mat4 Animator::ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
    to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
    to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
    to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;
    return to;
}

const aiNodeAnim* Animator::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName) {
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if (std::string(nodeAnim->mNodeName.data) == nodeName) {
            return nodeAnim;
        }
    }
    return nullptr;
}

uint Animator::FindScaling(float animationTime, const aiNodeAnim* pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (animationTime < pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    return 0;
}

uint Animator::FindRotation(float animationTime, const aiNodeAnim* pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (animationTime < pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    return 0;
}

uint Animator::FindPosition(float animationTime, const aiNodeAnim* pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (animationTime < pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    return 0;
}

glm::vec3 Animator::InterpolateScaling(float animationTime, const aiNodeAnim* nodeAnim) {
    if (nodeAnim->mNumScalingKeys == 1) {
        return glm::vec3(nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y, nodeAnim->mScalingKeys[0].mValue.z);
    }

    uint scalingIndex = FindScaling(animationTime, nodeAnim);
    uint nextScalingIndex = scalingIndex + 1;
    float deltaTime = nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime;
    float factor = (animationTime - nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
    const aiVector3D& start = nodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIndex].mValue;
    aiVector3D delta = end - start;
    aiVector3D interpolated = start + factor * delta;
    return glm::vec3(interpolated.x, interpolated.y, interpolated.z);
}

glm::quat Animator::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim) {
    if (nodeAnim->mNumRotationKeys == 1) {
        return glm::quat(nodeAnim->mRotationKeys[0].mValue.w, nodeAnim->mRotationKeys[0].mValue.x, nodeAnim->mRotationKeys[0].mValue.y, nodeAnim->mRotationKeys[0].mValue.z);
    }

    uint rotationIndex = FindRotation(animationTime, nodeAnim);
    uint nextRotationIndex = rotationIndex + 1;
    float deltaTime = nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime;
    float factor = (animationTime - nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
    const aiQuaternion& start = nodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& end = nodeAnim->mRotationKeys[nextRotationIndex].mValue;
    aiQuaternion result;
    aiQuaternion::Interpolate(result, start, end, factor);
    result = result.Normalize();
    return glm::quat(result.w, result.x, result.y, result.z);
}

glm::vec3 Animator::InterpolatePosition(float animationTime, const aiNodeAnim* nodeAnim) {
    if (nodeAnim->mNumPositionKeys == 1) {
        return glm::vec3(nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z);
    }

    uint positionIndex = FindPosition(animationTime, nodeAnim);
    uint nextPositionIndex = positionIndex + 1;
    float deltaTime = nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime;
    float factor = (animationTime - nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
    const aiVector3D& start = nodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIndex].mValue;
    aiVector3D delta = end - start;
    aiVector3D interpolated = start + factor * delta;
    return glm::vec3(interpolated.x, interpolated.y, interpolated.z);
}
