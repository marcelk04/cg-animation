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
    : App(1200, 800),
      cam(std::make_shared<MovingCamera>(glm::vec3(0.0f, 1.0f, 2.0f),
      glm::vec3(0.0f, 1.0f, 0.0f))),
      renderer(cam, resolution) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    scene = std::make_shared<Scene>();

    cam->setResolution(resolution);

    loadShaders();

    createMaterials();

    loadObjects();

    createLights();

    createRenderObjects();

    renderer.setScene(scene);
    renderer.updateCamUniforms();
}

void MainApp::init() {
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    Common::randomSeed();
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (scene->getDirLight().has_value()) {
        if (ImGui::SphericalSlider("Light direction", lightDir)) {
            scene->getDirLight()->setDirection(lightDir);
            renderer.updateLightingUniforms();
        }
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
    renderer.update(delta);

    if (cam->updateIfChanged()) {
        renderer.updateCamUniforms();
    }

    renderer.draw();
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

void MainApp::loadShaders() {
    simpleGeom = std::make_shared<Program>();
    simpleGeom->load("simple_geometry.vert", "simple_geometry.frag");
    simpleGeomId = renderer.addProgram(simpleGeom);

    texturedGeom = std::make_shared<Program>();
    texturedGeom->load("textured_geometry.vert", "textured_geometry.frag");
    texturedGeom->bindTextureUnit("uDiffuseTexture", 0);
    texturedGeom->bindTextureUnit("uNormalTexture", 1);
    texturedGeomId = renderer.addProgram(texturedGeom);
}

void MainApp::loadObjects() {
    cube.load("meshes/cube.obj");
    plane.load("meshes/plane.obj");
    sphere.load("meshes/highpolysphere.obj");
    bunny.load("meshes/bunny.obj");
    //house.loadWithTangents("meshes/cottage.obj");
}

void MainApp::createMaterials() {
    lightMaterial = {
        glm::vec3(100.0f),
        0.0f
    };

    floorMaterial = {
        glm::vec3(0.7f, 0.7f, 0.7f),
        0.5f
    };

    leftWallMaterial = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        0.5f
    };

    backWallMaterial = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.5f
    };

    rightWallMaterial = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        0.5f
    };

    bunnyMaterial = {
        glm::vec3(1.0f, 0.5f, 0.31f),
        0.6f
    };
}

void MainApp::createLights() {
    lightDir = glm::vec3(0.1f, 1.0f, 0.5f);

    DirLight dirLight;
    dirLight.setDirection(lightDir);
    dirLight.setColor(glm::vec3(0.5f));
    scene->setDirLight(std::move(dirLight));

    PointLight light0;
    light0.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    light0.setColor(glm::vec3(1.0f));
    scene->addPointLight(std::move(light0));
}

void MainApp::createRenderObjects() {
    RenderObject floor(plane);
    floor.setScale(10.0f);
    floor.setMaterial(floorMaterial);
    scene->addRenderObject(std::move(floor), simpleGeomId);

    RenderObject leftWall(plane);
    leftWall.setScale(2.0f);
    leftWall.setPosition(glm::vec3(-2.0f, 2.0f, 0.0f));
    leftWall.setRotation(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    leftWall.setMaterial(leftWallMaterial);
    scene->addRenderObject(std::move(leftWall), simpleGeomId);

    RenderObject backWall(plane);
    backWall.setScale(2.0f);
    backWall.setPosition(glm::vec3(0.0f, 2.0f, -2.0f));
    backWall.setRotation(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    backWall.setMaterial(backWallMaterial);
    scene->addRenderObject(std::move(backWall), simpleGeomId);

    RenderObject rightWall(plane);
    rightWall.setScale(2.0f);
    rightWall.setPosition(glm::vec3(2.0f, 2.0f, 0.0f));
    rightWall.setRotation(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rightWall.setMaterial(rightWallMaterial);
    scene->addRenderObject(std::move(rightWall), simpleGeomId);

    RenderObject lightSphere0(sphere);
    lightSphere0.setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    lightSphere0.setScale(0.1f);
    lightSphere0.setMaterial(lightMaterial);
    scene->addRenderObject(std::move(lightSphere0), simpleGeomId);

    RenderObject bunnyObj(bunny);
    bunnyObj.setPosition(glm::vec3(-0.7f, 0.58f, -0.7f));
    bunnyObj.setScale(0.8f);
    bunnyObj.setMaterial(bunnyMaterial);
    scene->addRenderObject(std::move(bunnyObj), simpleGeomId);

    RenderObject cubeObj(cube);
    cubeObj.setPosition(glm::vec3(0.7, 1.0f, -0.7f));
    cubeObj.setScale(0.2f);
    cubeObj.setMaterial(bunnyMaterial);
    scene->addRenderObject(std::move(cubeObj), simpleGeomId);
}
