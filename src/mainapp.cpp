#include "mainapp.hpp"
#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"
#include "framework/imguiutil.hpp"

#include <iostream>
#include <vector>

ParticleSystem::ParticleSystem() {
    shader.load("particles.vert", "particles.frag");
}

void ParticleSystem::init() {
    particles.resize(1000);
    for (auto& p : particles) {
        p.position = glm::vec3(0.0f);
        p.velocity = glm::vec3((rand() % 100 - 50) / 50.0f, (rand() % 100) / 50.0f, (rand() % 100 - 50) / 50.0f);
        p.lifetime = (rand() % 100) / 10.0f;
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

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro");
    App::setVSync(true);

    mesh.load("meshes/bunny.obj");
    fireshader.load("fireshader.vert", "fireshader.frag");
    fireshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());

    lightDir = glm::vec3(1.0f);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    particleSystem.init();
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (ImGui::SphericalSlider("Light Direction", lightDir)) {
        fireshader.set("uLightDir", lightDir);
    }

    ImGui::SliderFloat("t", &t, 0.0f, 1.0f);
}

void MainApp::render() {
    glm::vec3 pos = deCasteljau(spline, t);

    coolCamera.moveTo(pos);
    coolCamera.lookAt(glm::vec3(0.0f));

    if (coolCamera.updateIfChanged()) {
        fireshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fireshader.bind();
    mesh.draw();

    particleSystem.update(static_cast<float>(glfwGetTime()));
    particleSystem.render(coolCamera.projection() * coolCamera.view());
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action != Action::REPEAT) return;

    if (key == Key::W) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Direction);
    } else if (key == Key::S) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Direction);
    } else if (key == Key::A) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Right);
    } else if (key == Key::D) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Right);
    } else if (key == Key::SPACE) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Up);
    } else if (key == Key::LEFT_SHIFT) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Up);
    }
}

void MainApp::scrollCallback(float amount) {
    coolCamera.zoom(0.1f * amount);
}

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) {
        coolCamera.rotate(0.002f * movement);
    }
}

glm::vec3 MainApp::deCasteljau(const std::vector<glm::vec3>& spline, float t) {
    std::vector<glm::vec3> points(spline.size());

    for (int i = 0; i < spline.size(); i++) {
        points[i] = glm::vec3(spline[i]);
    }

    int n = points.size();
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            points[i] = (1 - t) * points[i] + t * points[i + 1];
        }
    }

    return points[0];
}


