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

    void update(float animationTime);

    glm::mat4 getLocalTransform() const { return m_LocalTransform; }
    std::string getBoneName() const { return m_Name; }
    int getBoneID() const { return m_ID; }

    int getPositionIndex(float animationTime) const;
    int getRotationIndex(float animationTime) const;
    int getScaleIndex(float animationTime) const;

private:
    float getInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

    glm::mat4 interpolatePosition(float animationTime) const;
    glm::mat4 interpolateRotation(float animationTime) const;
    glm::mat4 interpolateScaling(float animationTime) const;

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

