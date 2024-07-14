#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <list>

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone {
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);

    void Update(float animationTime);
    glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() const { return m_ID; }

    int GetPositionIndex(float animationTime) const;
    int GetRotationIndex(float animationTime) const;
    int GetScaleIndex(float animationTime) const;

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

    glm::mat4 InterpolatePosition(float animationTime) const;
    glm::mat4 InterpolateRotation(float animationTime) const;
    glm::mat4 InterpolateScaling(float animationTime) const;

private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;

    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;
};

