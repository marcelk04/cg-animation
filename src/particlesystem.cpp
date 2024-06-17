
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <cstdlib>
#include "particlesystem.hpp"
ParticleSystem::ParticleSystem() {
    shader.load("particles.vert", "particles.frag");
}

float generateLifetime() {
    float lambda = 1.0f / 1.0f; // Adjust the rate parameter for the exponential distribution
    float random = static_cast<float>(rand()) / RAND_MAX;
    return -log(1.0f - random) / lambda;
}

void ParticleSystem::init() {
    particles.resize(50000);
    for (auto& p : particles) {
        p.position = glm::vec3(0.0f);
        p.velocity = glm::vec3((rand() % 100 - 50) / 200.0f, (rand() % 100) / 200.0f, (rand() % 100 - 50) / 200.0f);
        p.lifetime = generateLifetime();
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
}

void ParticleSystem::update(float time) {
    shader.bind();
    shader.set("u_Time", time);
    shader.set("u_StartColor", glm::vec4(1.0, 0.5, 0.0, 1.0));
    shader.set("u_EndColor", glm::vec4(1.0, 0.0, 0.0, 0.0));
}

void ParticleSystem::render(const glm::mat4& viewProj) {
    shader.bind();
    shader.set("u_ViewProj", viewProj);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particles.size());
}
