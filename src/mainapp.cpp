#include "mainapp.hpp"

#include "framework/imguiutil.hpp"
#include "framework/common.hpp"
#include "renderer/renderobject.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "lightninggenerator.hpp"

#include <iostream>
#include <memory>
#include <typeinfo>

MainApp::MainApp()
        : App(1200, 800),
          cam(std::make_shared<MovingCamera>(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 5.0f, 0.0f))),
          renderer(cam, resolution),
          lightDir(glm::vec3(1.0f, 1.0f, 1.0f)),
          elapsedTime(0.0f),
          renderDuration(100.0f),
          soundPlayer(),
          soundPlayed(false)
{
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    cam->setResolution(resolution);

    ResourceManager::loadAnimationModel("rigged_model/sadly.dae", "sadly");
    ResourceManager::loadAnimation("rigged_model/sadly.dae", "sadly", "sadly_anim");
    animator = Animator(&ResourceManager::getAnimation("sadly_anim"));

    loadShaders();
    loadObjects();
    loadTextures();

    initParticleSystem();

    createMaterials();
    createLights();
    createRenderObjects();

    scenes.push_back(houseScene);
    scenes.push_back(sadScene);

    renderer.setScene(scenes[sceneIdx]);
    renderer.updateCamUniforms();

    if (!soundPlayer.init()) {
        std::cerr << "Failed to initialize SoundPlayer" << std::endl;
    }
}

void MainApp::init() {
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    Common::randomSeed();
}

void MainApp::resetRenderTimer(float duration) {
    elapsedTime = 0.0f;
    renderDuration = duration;
    soundPlayed = false; // Reset soundPlayed flag
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

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
//    std::cout << "elapsedTime: " << elapsedTime << std::endl;
//    if (elapsedTime < 50) {
//        if (!soundPlayed) {
//            soundPlayer.playSound("music/music.mp3"); // Start music playback
//            soundPlayed = true;
//        }

    renderer.setScene(scenes[sceneIdx]);

    renderer.update(delta);
    animator.update(delta);

    if (cam->updateIfChanged()) {
        renderer.updateCamUniforms();
    }

    const auto& transforms = animator.getFinalBoneMatrices();

    for (int i = 0; i < transforms.size(); i++) {
        animated->set("uFinalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }

    renderer.draw();
    elapsedTime += delta;

//    } else {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        soundPlayer.stopSound();
//
//    }
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 5.0f;

    if (action == Action::RELEASE) return;

    if (key == Key::W) {
        cam->move(delta * cameraSpeed * cam->getDirection());
    } else if (key == Key::S) {
        cam->move(-delta * cameraSpeed * cam->getDirection());
    } else if (key == Key::A) {
        cam->move(-delta * cameraSpeed * cam->getRight());
    } else if (key == Key::D) {
        cam->move(delta * cameraSpeed * cam->getRight());
    } else if (key == Key::SPACE) {
        cam->move(delta * cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f));
    } else if (key == Key::LEFT_SHIFT) {
        cam->move(delta * cameraSpeed * glm::vec3(0.0f, -1.0f, 0.0f));
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

    texturedGeomNormals = std::make_shared<Program>();
    texturedGeomNormals->load("textured_geometry_normals.vert", "textured_geometry_normals.frag");
    texturedGeomNormals->bindTextureUnit("uDiffuseTexture", 0);
    texturedGeomNormals->bindTextureUnit("uNormalTexture", 1);
    texturedGeomNormalsId = renderer.addProgram(texturedGeomNormals);

    texturedGeom = std::make_shared<Program>();
    texturedGeom->load("textured_geometry.vert", "textured_geometry.frag");
    texturedGeom->bindTextureUnit("uDiffuseTexture", 0);
    texturedGeomId = renderer.addProgram(texturedGeom);

    animated = std::make_shared<Program>();
    animated->load("assimpshader.vert", "assimpshader.frag");
    animatedId = renderer.addProgram(animated);
}

void MainApp::loadObjects() {
    ResourceManager::loadMesh("meshes/cube.obj", "cube");
    ResourceManager::loadMesh("meshes/plane.obj", "plane");
    ResourceManager::loadMesh("meshes/highpolysphere.obj", "sphere");
    ResourceManager::loadMesh("meshes/bunny.obj", "bunny");
    ResourceManager::loadMesh("meshes/cottage.obj", "house");
}

void MainApp::loadTextures() {
    ResourceManager::loadTexture("textures/cottage_diffuse.png", "house_diffuse");
    ResourceManager::loadTexture("textures/cottage_normal.png", "house_normal");
}

void MainApp::initParticleSystem() {
    ParticleSystem ps;
    ps.init();

    //scene->setParticleSystem(std::move(ps));
}

void MainApp::createMaterials() {

}

void MainApp::createLights() {
    lightDir = glm::vec3(0.1f, 1.0f, 0.5f);

    DirLight dirLight;
    dirLight.setDirection(lightDir);
    dirLight.setColor(glm::vec3(0.5f));
    houseScene->setDirLight(std::move(dirLight));

    PointLight light0;
    light0.setPosition(glm::vec3(20.0f, 20.0f, 20.0f));
    light0.setColor(glm::vec3(1.0f));
    houseScene->addPointLight(std::move(light0));
}

void MainApp::createRenderObjects() {
    RenderObject houseObj;
    houseObj.setMesh("house");
    houseObj.setDiffuseTexture("house_diffuse");
    houseObj.setNormalTexture("house_normal");
    houseObj.setScale(1.0f);
    houseScene->addRenderObject(std::move(houseObj), texturedGeomNormalsId);

    RenderObject houseObj2;
    houseObj2.setMesh("house");
    houseObj2.setDiffuseTexture("house_diffuse");
    houseObj2.setNormalTexture("house_normal");
    houseObj2.setScale(1.0f);
    sadScene->addRenderObject(std::move(houseObj2), texturedGeomNormalsId);

    RenderObject sadly;
    sadly.setAnimationModel("sadly");
    sadly.setPosition(glm::vec3(3.0f, 0.0f, 3.0f));
    sadScene->addRenderObject(std::move(sadly), animatedId);
}
