#pragma once

#include "framework/gl/buffer.hpp"
#include "framework/gl/vertexarray.hpp"
#include "framework/gl/program.hpp"

#include <glm/glm.hpp>

#include <vector>

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
        float type;
    };

    // kann theoretisch entfernt werden, da die Daten bereits auf GPU gespeichert sind
    std::vector<Particle> particles;

    VertexArray vao;
    Buffer vbo;

    Program shader;
};
