#include "animator.hpp"

#include "framework/common.hpp"

Animator::Animator(Animation* animation)
{
    m_CurrentTime = 0.0;
    m_CurrentAnimation = animation;

    m_FinalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++) {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimation(float dt)
{
    m_DeltaTime = dt;

    if (m_CurrentAnimation) {
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation *pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform)
{
    const std::string& nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* bone = m_CurrentAnimation->FindBone(nodeName);

    if (bone)
    {
        bone->Update(m_CurrentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    const auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        const int index = boneInfoMap.at(nodeName).id;
        const glm::mat4& offset = boneInfoMap.at(nodeName).offset;

        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++) {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}
