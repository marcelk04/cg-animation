#include "particlesystem.hpp"

#include "framework/common.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cstdlib>
#include <iostream>

ParticleSystem::ParticleSystem() {
    shader.load("particleshader.vert", "particleshader.frag");
}

float generateLifetime() {
    float lambda = 1.0f / 10.0f; // Adjust the rate parameter for the exponential distribution
    float random = Common::randomFloat();
    return -log(1.0f - random) / lambda;
}

void ParticleSystem::init() {
    particles.resize(60000);

    int i = 0;

    for (auto& p : particles) {
        i++;

        float x = Common::randomFloat(-19.0f, 13.0f);
        float y = Common::randomFloat(0.0f, 15.0f);
        float z = Common::randomFloat(-15.0f, 17.0f);

        float vx = Common::randomFloat(-0.25f, 0.25f);
        float vy = Common::randomFloat(0.0f, 0.5f);
        float vz = Common::randomFloat(-0.25f, 0.25f);

        p.position = glm::vec3(x,y,z);
        p.velocity = glm::vec3(vx, vy, vz);
        p.lifetime = generateLifetime();
        p.type = (i % 4 == 0) ? 1.0f : 0.0f;
    }

    glPointSize(2.5f);

    vao.bind();
    vbo.load(Buffer::Type::ARRAY_BUFFER, particles);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,  sizeof(Particle), (void*)offsetof(Particle, type));
}

void ParticleSystem::update(float time) {
    shader.set("u_Time", time);
}

void ParticleSystem::render(const glm::mat4& viewProj) {
    shader.bind();
    shader.set("u_ViewProj", viewProj);

    vao.bind();
    glDrawArrays(GL_POINTS, 0, particles.size());
}
