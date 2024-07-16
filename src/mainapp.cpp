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
          cam(std::make_shared<MovingCamera>(glm::vec3(10.0f, 10.0f, 40.0f), glm::vec3(0.0f, 5.0f, 10.0f))),
          renderer(cam, resolution),
          lightDir(glm::vec3(1.0f, 1.0f, 1.0f)),
          elapsedTime(0.0f),
          renderDuration(100.0f),
          soundPlayer(),
          soundPlayed(false)
{
    Common::randomSeed();

    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    houseScene = std::make_shared<Scene>();
    lightningScene = std::make_shared<Scene>();
    sadScene = std::make_shared<Scene>();

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
    scenes.push_back(lightningScene);
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

    if (elapsedTime > 5.0f && elapsedTime < 5.5f) {
        sceneIdx = 1;
    }

    if (elapsedTime > 6.0f && elapsedTime < 6.5f) {
        sceneIdx = 2;
        Animation* anim = &ResourceManager::getAnimation("sadly_anim");
        animator.playAnimation(anim);
    }

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

    auto lightningData = LightningGenerator::genBolt(glm::vec3(-5.0f, 25.0f, 15.0f), glm::vec3(0.0f, 10.0f, 5.0f), 6, cam->getDirection());
    auto lightningMeshData = LightningGenerator::genMeshData(lightningData, cam->getDirection());
    Mesh lightningMesh;
    lightningMesh.load(lightningMeshData.first, lightningMeshData.second);
    ResourceManager::addMesh(std::move(lightningMesh), "lightning");
}

void MainApp::loadTextures() {
    ResourceManager::loadTexture("textures/cottage_diffuse.png", "house_diffuse");
    ResourceManager::loadTexture("textures/cottage_normal.png", "house_normal");
}

void MainApp::initParticleSystem() {
    ParticleSystem ps;
    ps.init();

    sadScene->setParticleSystem(std::move(ps));
}

void MainApp::createMaterials() {
    lightningMaterial = {
        glm::vec3(5.0f, 5.0f, 7.0f),
        0.0f
    };
}

void MainApp::createLights() {
    lightDir = glm::vec3(0.1f, 1.0f, 0.5f);

    DirLight dirLight0;
    dirLight0.setDirection(lightDir);
    dirLight0.setColor(glm::vec3(0.5f));
    houseScene->setDirLight(std::move(dirLight0));

    DirLight dirLight1;
    dirLight1.setDirection(lightDir);
    dirLight1.setColor(glm::vec3(0.5f));
    lightningScene->setDirLight(std::move(dirLight1));

    DirLight dirLight2;
    dirLight2.setDirection(lightDir);
    dirLight2.setColor(glm::vec3(0.5f));
    sadScene->setDirLight(std::move(dirLight2));

    PointLight light0;
    light0.setPosition(glm::vec3(20.0f, 20.0f, 20.0f));
    light0.setColor(glm::vec3(1.0f));
    //houseScene->addPointLight(std::move(light0));
}

void MainApp::createRenderObjects() {
    RenderObject houseObj;
    houseObj.setMesh("house");
    houseObj.setDiffuseTexture("house_diffuse");
    houseObj.setNormalTexture("house_normal");
    houseObj.setScale(1.0f);
    houseScene->addRenderObject(std::move(houseObj), texturedGeomNormalsId);

    RenderObject houseObj1;
    houseObj1.setMesh("house");
    houseObj1.setDiffuseTexture("house_diffuse");
    houseObj1.setNormalTexture("house_normal");
    houseObj1.setScale(1.0f);
    lightningScene->addRenderObject(std::move(houseObj1), texturedGeomNormalsId);

    RenderObject lightningObj;
    lightningObj.setMesh("lightning");
    lightningObj.setMaterial(lightningMaterial);
    lightningScene->addRenderObject(std::move(lightningObj), simpleGeomId);

    RenderObject houseObj2;
    houseObj2.setMesh("house");
    houseObj2.setDiffuseTexture("house_diffuse");
    houseObj2.setNormalTexture("house_normal");
    houseObj2.setScale(1.0f);
    sadScene->addRenderObject(std::move(houseObj2), texturedGeomNormalsId);

    RenderObject sadly;
    sadly.setAnimationModel("sadly");
    sadly.setPosition(glm::vec3(0.0f, 0.0f, 30.0f));
    sadly.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    sadScene->addRenderObject(std::move(sadly), animatedId);
}
