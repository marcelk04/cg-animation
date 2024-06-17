#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "framework/gl/program.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    void init();
    void update(float time);
    void render(const glm::mat4& viewProj);

private:
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        float lifetime;
    };

    std::vector<Particle> particles;
    GLuint vao, vbo;
    Program shader;
};
