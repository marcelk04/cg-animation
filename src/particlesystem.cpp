
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <cstdlib>
#include "particlesystem.hpp"
#include <iostream>
ParticleSystem::ParticleSystem() {
    shader.load("particles.vert", "particles.frag");
}

float generateLifetime() {
    float lambda = 1.0f / 10.0f; // Adjust the rate parameter for the exponential distribution
    float random = static_cast<float>(rand()) / RAND_MAX;
    return -log(1.0f - random) / lambda;
}

void ParticleSystem::init() {
    std::cout<<"init particles\n";
    particles.resize(200000);
    int i = 0;
    for (auto& p : particles) {
        i++;

        float x = random_float(-17,13);
        float y = random_float(0,15);
        float z = random_float(-15,15);
        p.position = glm::vec3(x,y,z);
        p.velocity = glm::vec3((rand() % 100 - 50) / 200.0f, (rand() % 100) / 200.0f, (rand() % 100 - 50) / 200.0f);
        p.lifetime = generateLifetime();
        p.type = (i % 4 == 0) ? 1 : 0;
    }
    glPointSize(2.5f);
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
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_INT,GL_FALSE,  sizeof(Particle), (void*)offsetof(Particle, type));
}

void ParticleSystem::update(float time) {
    std::cout<<"update particles\n";
    shader.bind();
    shader.set("u_Time", time);
    }


void ParticleSystem::render(const glm::mat4& viewProj) {
    shader.bind();
    shader.set("u_ViewProj", viewProj);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particles.size());
}
