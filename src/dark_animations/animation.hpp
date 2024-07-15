#pragma once

#include "dark_animations/bone.hpp"
#include "dark_animations/animationmodel.hpp"
#include "framework/mesh.hpp"

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <vector>
#include <map>
#include <functional>

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    Animation() = default;
    Animation(const std::string& animationPath, AnimationModel* model);

    Bone* findBone(const std::string& name);

    float getTicksPerSecond() const { return m_TicksPerSecond; }
    float getDuration() const { return m_Duration;}
    const AssimpNodeData& getRootNode() const { return m_RootNode; }
    const std::map<std::string,BoneInfo>& getBoneIDMap() const { return m_BoneInfoMap; }

private:
    void readMissingBones(const aiAnimation* animation, AnimationModel& model);
    void readHierarchyData(AssimpNodeData& dest, const aiNode* src) const;

private:
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};