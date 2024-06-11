#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/imguiutil.hpp"

#include <iostream>

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    mesh.load("meshes/bunny.obj");
    meshshader.load("meshshader.vert", "meshshader.frag");
    meshshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());

    lightDir = glm::vec3(1.0f);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (ImGui::SphericalSlider("Light Direction", lightDir)) {
        meshshader.set("uLightDir", lightDir);
    }
}

void MainApp::render() {
    if (coolCamera.updateIfChanged()) {
        meshshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    meshshader.bind();
    mesh.draw();
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action != Action::REPEAT) return;

    if (key == Key::W) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Direction);
    }
    else if (key == Key::S) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Direction);
    }
    else if (key == Key::A) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Right);
    }
    else if (key == Key::D) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Right);
    }
    else if (key == Key::SPACE) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Up);
    }
    else if (key == Key::LEFT_SHIFT) {
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
