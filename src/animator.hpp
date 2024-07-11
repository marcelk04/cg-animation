#pragma once
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include "mesh.hpp"  // Include mesh.hpp to use the BoneInfo struct

class Animator {
public:
    Animator(const aiAnimation* animation, const aiNode* rootNode);
    void UpdateAnimation(float dt);
    std::vector<glm::mat4> GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }

private:
    void CalculateBoneTransform(const aiNode* node, glm::mat4 parentTransform, float animationTime);
    glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
    glm::vec3 InterpolateScaling(float animationTime, const aiNodeAnim* nodeAnim);
    glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
    glm::vec3 InterpolatePosition(float animationTime, const aiNodeAnim* nodeAnim);
    uint FindScaling(float animationTime, const aiNodeAnim* pNodeAnim);
    uint FindRotation(float animationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float animationTime, const aiNodeAnim* pNodeAnim);

    const aiAnimation* m_CurrentAnimation;
    const aiNode* m_RootNode;
    float m_CurrentTime;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCount;
};
