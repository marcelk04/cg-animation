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

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

MainApp::MainApp()
    : App(800, 600), cam(std::make_shared<MovingCamera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f))), renderer(cam) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    cam->setResolution(resolution);

    lightingshader = std::make_shared<Program>();
    lightingshader->load("lightingshader.vert", "lightingshader.frag");
    size_t lightingshaderId = renderer.addProgram(lightingshader);

    meshshader = std::make_shared<Program>();
    meshshader->load("meshshader.vert", "meshshader.frag");
    size_t meshshaderId = renderer.addProgram(meshshader);

    colorshader = std::make_shared<Program>();
    colorshader->load("colorshader.vert", "colorshader.frag");
    size_t colorshaderId = renderer.addProgram(colorshader);

    cube.load("meshes/cube.obj");

    Material material{
        glm::vec3(1.0f, 0.5f, 0.31f),
        glm::vec3(1.0f, 0.5f, 0.31f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        32.0f
    };

    RenderObject normalCube(cube);
    normalCube.setPositionAndSize(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f);
    normalCube.setMaterial(material);
    renderer.addObject(std::move(normalCube), lightingshaderId);

    DirLight dirLight;
    dirLight.direction = glm::vec3(0.1f, 1.0f, 0.5f);
    dirLight.ambient = glm::vec3(0.2f);
    dirLight.diffuse = glm::vec3(0.5f);
    dirLight.specular = glm::vec3(1.0f);

    renderer.setDirLight(std::move(dirLight));

    for (size_t i = 0; i < 4; i++) {
        PointLight pointLight;
        pointLight.position = lightPositions[i];
        pointLight.ambient = glm::vec3(0.2f);
        pointLight.diffuse = glm::vec3(0.5f);
        pointLight.specular = glm::vec3(1.0f);
        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        renderer.setPointLight(std::move(pointLight), i);

        RenderObject lightCube(cube);
        lightCube.setPositionAndSize(lightPositions[i], 0.2f);
        lightCube.setColor(glm::vec3(1.0f));
        renderer.addObject(std::move(lightCube), colorshaderId);
    }

    renderer.updateLightingUniforms();
    renderer.updateCamUniforms();
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);
}

void MainApp::render() {
    if (cam->updateIfChanged()) {
        renderer.updateCamUniforms();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer.draw();
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action != Action::REPEAT) return;

    if (key == Key::W) {
        cam->move(delta * cameraSpeed * cam->getDirection());
    }
    else if (key == Key::S) {
        cam->move(-delta * cameraSpeed * cam->getDirection());
    }
    else if (key == Key::A) {
        cam->move(-delta * cameraSpeed * cam->getRight());
    }
    else if (key == Key::D) {
        cam->move(delta * cameraSpeed * cam->getRight());
    }
    else if (key == Key::SPACE) {
        cam->move(delta * cameraSpeed * cam->getUp());
    }
    else if (key == Key::LEFT_SHIFT) {
        cam->move(-delta * cameraSpeed * cam->getUp());
    }
}

void MainApp::scrollCallback(float amount) {
    cam->zoom(0.1f * amount);
}

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) {
        cam->rotate(0.002f * movement);
    }
}

void MainApp::resizeCallback(const glm::vec2& resolution) {
    cam->setResolution(resolution);
}

