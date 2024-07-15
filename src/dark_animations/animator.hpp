#pragma once

#include "dark_animations/bone.hpp"
#include "dark_animations/animation.hpp"

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <map>
#include <vector>

class Animator {
public:
    Animator();
    Animator(Animation* animation);

    void update(float dt);

    void playAnimation(Animation* animation);

    void calculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform);
    
    const std::vector<glm::mat4>& getFinalBoneMatrices() const { return m_FinalBoneMatrices; }

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;

    float m_CurrentTime;
};
