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
    : App(800, 600), cam(std::make_shared<MovingCamera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f))), renderer(cam, resolution) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    cam->setResolution(resolution);

    geometryshader = std::make_shared<Program>();
    geometryshader->load("deferred_geometry.vert", "deferred_geometry.frag");
    geometryshaderId = renderer.addProgram(geometryshader);

    Material material0 {
        glm::vec3(1.0f, 0.5f, 0.31f),
        0.4f
    };

    Material material1 {
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

    auto segments = LightningGenerator::genBolt(glm::vec3(1.0f, 4.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), 5, cam->getDirection());
    auto meshdata = LightningGenerator::genMeshData(segments, cam->getDirection());

    lightningMesh.load(meshdata.first, meshdata.second);

    RenderObject lightningBolt(lightningMesh);
    lightningBolt.setMaterial(lightningMaterial);
    scene.addRenderObject(std::move(lightningBolt), geometryshaderId);

    PointLight lightningLight;
    lightningLight.setPosition(glm::vec3(1.0f, -0.9f, 0.0f));
    lightningLight.setColor(glm::vec3(1.0f));
    scene.addPointLight(std::move(lightningLight));

    RenderObject normalCube(cube);
    normalCube.setPositionAndSize(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f);
    normalCube.setMaterial(material1);
    scene.addRenderObject(std::move(normalCube), geometryshaderId);

    RenderObject planeObj(plane);
    planeObj.setPositionAndSize(glm::vec3(0.0f, -1.0f, 0.0f), 20.0f);
    planeObj.setMaterial(material0);
    scene.addRenderObject(std::move(planeObj), geometryshaderId);

    DirLight dirLight;
    dirLight.setDirection(glm::vec3(0.1f, 1.0f, 0.5f));
    dirLight.setColor(glm::vec3(0.2f));
    scene.setDirLight(std::move(dirLight));

    for (size_t i = 0; i < 4; i++) {
        PointLight pointLight;
        pointLight.setPosition(lightPositions[i]);
        pointLight.setColor(glm::vec3(1.5f));
        scene.addPointLight(std::move(pointLight));

        RenderObject lightCube(sphere);
        lightCube.setPositionAndSize(lightPositions[i], 0.1f);
        lightCube.setMaterial(lightMaterial);
        scene.addRenderObject(std::move(lightCube), geometryshaderId);
    }

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
    else if (key == Key::G) {
        auto segments = LightningGenerator::genBolt(glm::vec3(1.0f, 4.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), 5, cam->getDirection());
        auto meshdata = LightningGenerator::genMeshData(segments, cam->getDirection());

        lightningMesh.load(meshdata.first, meshdata.second);
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
