#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/imguiutil.hpp"

#include <iostream>
#include <memory>

MainApp::MainApp() 
    : App(800, 600) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    cam.setResolution(resolution);

    cube.load("meshes/cube.obj");

    normalCube = std::make_shared<RenderObject>(cube, glm::vec3(1.0f, 0.5f, 0.31f));
    lightCube = std::make_shared<RenderObject>(cube, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(2.0f, 1.0f, -5.0f), 0.2f);

    meshshader.load("meshshader.vert", "meshshader.frag");
    meshshader.set("uWorldToClip", cam.projection() * cam.view());

    lightingshader.load("lightingshader.vert", "lightingshader.frag");
    lightingshader.set("uWorldToClip", cam.projection() * cam.view());
    lightingshader.set("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    colorshader.load("colorshader.vert", "colorshader.frag");
    colorshader.set("uWorldToClip", cam.projection() * cam.view());
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);
}

void MainApp::render() {
    if (cam.updateIfChanged()) {
        meshshader.set("uWorldToClip", cam.projection() * cam.view());
        lightingshader.set("uWorldToClip", cam.projection() * cam.view());
        colorshader.set("uWorldToClip", cam.projection() * cam.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    normalCube->draw(lightingshader);
    lightCube->draw(colorshader);
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action != Action::REPEAT) return;

    if (key == Key::W) {
        cam.move(delta * cameraSpeed * cam.m_Direction);
    }
    else if (key == Key::S) {
        cam.move(-delta * cameraSpeed * cam.m_Direction);
    }
    else if (key == Key::A) {
        cam.move(-delta * cameraSpeed * cam.m_Right);
    }
    else if (key == Key::D) {
        cam.move(delta * cameraSpeed * cam.m_Right);
    }
    else if (key == Key::SPACE) {
        cam.move(delta * cameraSpeed * cam.m_Up);
    }
    else if (key == Key::LEFT_SHIFT) {
        cam.move(-delta * cameraSpeed * cam.m_Up);
    }
}

void MainApp::scrollCallback(float amount) {
    cam.zoom(0.1f * amount);
}

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) {
        cam.rotate(0.002f * movement);
    }
}

void MainApp::resizeCallback(const glm::vec2& resolution) {
    cam.setResolution(resolution);
}

