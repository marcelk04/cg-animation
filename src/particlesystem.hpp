#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "framework/gl/program.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    //~ParticleSystem();
    void init();
    void update(float time);
    void render(const glm::mat4& viewProj);
    float random_float(float min, float max) { return ((float)rand() / RAND_MAX) * (max - min) + min; }

private:
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        float lifetime;
        int type;
    };

    std::vector<Particle> particles;
    GLuint vao, vbo;
    Program shader;
};
