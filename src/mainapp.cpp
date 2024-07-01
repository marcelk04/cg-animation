#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/imguiutil.hpp"
#include "framework/common.hpp"

#include "lightninggenerator.hpp"

#include <iostream>
#include <memory>

MainApp::MainApp()
    : App(1200, 800), cam(std::make_shared<MovingCamera>(glm::vec3(0.0f, 2.0f, 4.0f), glm::vec3(0.0f))), renderer(cam, resolution) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    cam->setResolution(resolution);

    geometryshader = std::make_shared<Program>();
    geometryshader->load("deferred_geometry.vert", "deferred_geometry.frag");
    geometryshaderId = renderer.addProgram(geometryshader);

    Material floorMaterial {
        glm::vec3(1.0f, 0.5f, 0.31f),
        0.4f
    };

    Material bunnyMaterial {
        glm::vec3(0.6f, 0.2f, 0.4f),
        0.4f
    };

    Material lightMaterial {
        glm::vec3(100.0f),
        0.0f
    };

    Material lightningMaterial{
        glm::vec3(5.0f, 5.0f, 10.0f),
        0.0f
    };

    cube.load("meshes/cube.obj");
    plane.load("meshes/plane.obj");
    sphere.load("meshes/highpolysphere.obj");
    bunny.load("meshes/bunny.obj");

    RenderObject planeObj(plane);
    planeObj.setPositionAndSize(glm::vec3(0.0f, -1.0f, 0.0f), 20.0f);
    planeObj.setMaterial(floorMaterial);
    scene.addRenderObject(std::move(planeObj), geometryshaderId);

    RenderObject bunnyObj(bunny);
    bunnyObj.setPosition(glm::vec3(0.0f, -0.36, 0.0f));
    bunnyObj.setMaterial(bunnyMaterial);
    scene.addRenderObject(std::move(bunnyObj), geometryshaderId);

    lightDir =  glm::vec3(0.1f, 1.0f, 0.5f);

    DirLight dirLight;
    dirLight.setDirection(lightDir);
    dirLight.setColor(glm::vec3(0.5f));
    scene.setDirLight(std::move(dirLight));

    PointLight light0;
    light0.setPosition(glm::vec3(-2.0f, 3.0f, -1.0f));
    light0.setColor(glm::vec3(1.5f));
    scene.addPointLight(std::move(light0));

    RenderObject lightSphere0(sphere);
    lightSphere0.setPositionAndSize(glm::vec3(-2.0f, 3.0f, -1.0f), 0.1f);
    lightSphere0.setMaterial(lightMaterial);
    scene.addRenderObject(std::move(lightSphere0), geometryshaderId);

    renderer.updateLightingUniforms(scene);
    renderer.updateCamUniforms();
}

void MainApp::init() {
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    Common::randomSeed();
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (ImGui::SphericalSlider("Light direction", lightDir)) {
        scene.getDirLight().value().setDirection(lightDir);
        renderer.updateLightingUniforms(scene);
    }

    float exposure = renderer.getExposure();
    ImGui::SliderFloat("Exposure", &exposure, 0.0f, 2.0f);
    renderer.setExposure(exposure);

    float gamma = renderer.getGamma();
    ImGui::SliderFloat("Gamma", &gamma, 0.5f, 5.0f);
    renderer.setGamma(gamma);

    int blurAmount = renderer.getBlurAmount();
    ImGui::SliderInt("Blur Amount", &blurAmount, 2, 20);
    renderer.setBlurAmount(blurAmount);
}

void MainApp::render() {
    if (cam->updateIfChanged()) {
        renderer.updateCamUniforms();
    }

    renderer.draw(scene);
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action == Action::RELEASE) return;

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
    renderer.setResolution(resolution);
}
