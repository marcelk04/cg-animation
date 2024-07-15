#include "animator.hpp"

#include "framework/common.hpp"

Animator::Animator(Animation* animation) {
    m_CurrentAnimation = animation;
    m_CurrentTime = 0.0f;
    m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
}

void Animator::update(float dt) {
    if (m_CurrentAnimation) {
        m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration()); // loop animation

        calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
    }
}

void Animator::playAnimation(Animation* animation) {
    m_CurrentAnimation = animation;
    m_CurrentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform) {
    const std::string& nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* bone = m_CurrentAnimation->findBone(nodeName);

    if (bone) {
        bone->update(m_CurrentTime);

        nodeTransform = bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    const auto& boneInfoMap = m_CurrentAnimation->getBoneIDMap();

    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        const int index = boneInfoMap.at(nodeName).id;
        const glm::mat4& offset = boneInfoMap.at(nodeName).offset;

        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (const auto& child : node->children) {
        calculateBoneTransform(&child, globalTransformation);
    }
}
