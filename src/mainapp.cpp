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

    scene0 = std::make_shared<Scene>();
    scene1 = std::make_shared<Scene>();
    scene2 = std::make_shared<Scene>();
    scene3 = std::make_shared<Scene>();
    scene4 = std::make_shared<Scene>();
    scene5 = std::make_shared<Scene>();
    scene6 = std::make_shared<Scene>();

    cam->setResolution(resolution);

    ResourceManager::loadAnimationModel("rigged_model/happy.dae", "happy_boy");
    ResourceManager::loadAnimation("rigged_model/happy.dae", "happy_boy", "happy_boy_anim");

    ResourceManager::loadAnimationModel("rigged_model/sadly.dae", "sad_boy");
    ResourceManager::loadAnimation("rigged_model/sadly.dae", "sad_boy", "sad_boy_anim");
        //animator.playAnimation(&ResourceManager::getAnimation("happy_boy_anim"));

    loadShaders();
    loadObjects();
    loadTextures();

    initParticleSystem();

    createCameraPaths();
    createMaterials();
    createLights();
    createRenderObjects();

    scenes.push_back(scene0);
    scenes.push_back(scene1);
    scenes.push_back(scene2);
    scenes.push_back(scene3);
    scenes.push_back(scene4);
    scenes.push_back(scene5);
    scenes.push_back(scene6);

    if (sceneIdx != -1)
        renderer.setScene(scenes[sceneIdx]);

    renderer.updateCamUniforms();
    //renderer.showCameraControlPoints(true);

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

void MainApp::render() {
    //std::cout << "elapsedTime: " << elapsedTime << std::endl;
//    if (elapsedTime < 50) {
//        if (!soundPlayed) {
//            soundPlayer.playSound("music/music.mp3"); // Start music playback
//            soundPlayed = true;
//        }

    if (sceneIdx == 5 && elapsedTime > currSceneStart + sceneDuration[sceneIdx]) {
        sceneIdx = 6;
        currSceneStart = elapsedTime;
        std::cout << "Loading scene 6\n";

        animator.playAnimation(&ResourceManager::getAnimation("happy_boy_anim"));
    }
    else if (sceneIdx == 4 && elapsedTime > currSceneStart + sceneDuration[sceneIdx]) {
        sceneIdx = 5;
        currSceneStart = elapsedTime;
        std::cout << "Loading scene 5\n";
        soundPlayer.stopSound();
        soundPlayer.playSound("music/party.mp3");

        animator.playAnimation(&ResourceManager::getAnimation("happy_boy_anim"));
    }
    else if (sceneIdx == 2 && elapsedTime > currSceneStart + sceneDuration[sceneIdx]) {
        sceneIdx = 4;
        currSceneStart = elapsedTime;
        std::cout << "Loading scene 4\n";
        soundPlayer.stopSound();
        soundPlayer.playSound("music/hope.mp3");
        animator.playAnimation(nullptr);
    }
    else if (sceneIdx == 1 && elapsedTime > currSceneStart + sceneDuration[sceneIdx]) {
        sceneIdx = 2;
        currSceneStart = elapsedTime;
      
        soundPlayer.stopSound();
        soundPlayer.playSound("music/sad.mp3");
      
        std::cout << "Loading scene 2\n";

        animator.playAnimation(&ResourceManager::getAnimation("sad_boy_anim"));
    }
    else if (sceneIdx == 0 && elapsedTime > currSceneStart + sceneDuration[sceneIdx]) {
        sceneIdx = 1;
        currSceneStart = elapsedTime;
      
        soundPlayer.stopSound();
        soundPlayer.playSound("music/storm lightning and thunder sound effect.mp3");

        std::cout << "Loading scene 1\n";

        animator.playAnimation(nullptr);
    }
    else if (elapsedTime >= 0.0f && sceneIdx == -1) {
        sceneIdx = 0;
        currSceneStart = elapsedTime;
        std::cout << "Loading scene 0\n";

        animator.playAnimation(&ResourceManager::getAnimation("happy_boy_anim"));
    }

    if(elapsedTime > 0.0f && sceneIdx == 0 && animationRunning && !soundPlayed){
        soundPlayer.playSound("music/music.mp3");
        soundPlayed = true;

    }
    // update lightning meshes
    if (sceneIdx == 1) {
        if (elapsedTime - currSceneStart > 0.3f) scene1->getRenderObject(simpleGeomId, lightningObjId).setMesh("lightning1");
        if (elapsedTime - currSceneStart > 0.6f) scene1->getRenderObject(simpleGeomId, lightningObjId).setMesh("lightning2");
        if (elapsedTime - currSceneStart > 0.9f)  {
            scene1->removeRenderObject(simpleGeomId, lightningObjId);
            scene1->removePointLight(0);
        }
    }

    renderer.setScene(scenes[sceneIdx]);
    scenes[sceneIdx]->getCameraController()->setEnabled(animationRunning);

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
  
    if (animationRunning) {
        elapsedTime += delta;
    }
  
    if (elapsedTime > 100.0f) {
        soundPlayer.stopSound();
    }

//    } else {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        soundPlayer.stopSound();
//
//    }
}


//void MainApp::buildImGui() {
//    if (ImGui::SphericalSlider("Light direction", lightDir)) {
//        scene0->getDirLight()->setDirection(lightDir);
//        renderer.updateLightingUniforms();
//    }
//}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 25.0f;

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
    } else if (key == Key::F) {
        animationRunning = !animationRunning;
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

    tiledGeom = std::make_shared<Program>();
    tiledGeom->load("tiled_textured_geometry.vert", "tiled_textured_geometry.frag");
    tiledGeom->bindTextureUnit("uDiffuseTexture", 0);
    tiledGeom->set("uTileFactor", 40.0f);
    tiledGeomId = renderer.addProgram(tiledGeom);
}

void MainApp::loadObjects() {
    ResourceManager::loadMesh("meshes/static_book.obj", "cube");
    ResourceManager::loadMesh("meshes/plane.obj", "plane");
    ResourceManager::loadMesh("meshes/highpolysphere.obj", "sphere");
    ResourceManager::loadMesh("meshes/bunny.obj", "bunny");
    ResourceManager::loadMesh("meshes/cottage.obj", "house");
    ResourceManager::loadMesh("meshes/ruined_building.obj", "ruin");

    for (uint32_t i = 0; i < 3; i++) { // generate 3 bolts
        auto lightningMeshData = LightningGenerator::genMeshData(glm::vec3(-5.0f, 60.0f, 15.0f), glm::vec3(0.0f, 10.0f, 5.0f), 7, cam->getDirection());
        Mesh lightningMesh;
        lightningMesh.load(lightningMeshData.first, lightningMeshData.second);
        ResourceManager::addMesh(std::move(lightningMesh), "lightning" + std::to_string(i));
    }

}

void MainApp::loadTextures() {
    ResourceManager::loadTexture("textures/cottage_diffuse.png", "house_diffuse");
    ResourceManager::loadTexture("textures/cottage_normal.png", "house_normal");
    ResourceManager::loadTexture("textures/text.jpg", "ruin_diffuse");
    ResourceManager::loadTexture("textures/normalmap.jpg", "ruin_normal");
    ResourceManager::loadTexture("textures/superbible.jpg", "superbible");
    ResourceManager::loadTexture("textures/grass_tileable.jpg", "grass");
}

void MainApp::initParticleSystem() {
    ParticleSystem ps;
    ps.init();

    scene2->setParticleSystem(std::move(ps));
}

void MainApp::createCameraPaths() {
    // scene 0

    Spline sc0;
    sc0.addCurve({
        glm::vec3(30.0f, 60.0f, 0.0f),
        glm::vec3(30.0f, 20.0f, 10.0f),
        glm::vec3(20.0f, 5.0f, 40.0f),
        glm::vec3(10.0f, 5.0f, 50.0f)
    });

    sc0.addCurve({
        glm::vec3(-2.0f, 30.0f, 50.0f),
        glm::vec3(-10.0f, 20.0f, 40.0f)
    });

    Spline se0;
    se0.addCurve({
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(0.0f, 3.0f, 40.0f),
        glm::vec3(0.0f, 3.0f, 40.0f)
    });

    se0.addCurve({
        glm::vec3(0.0f, 30.0f, 0.0f)
    });

    CameraController c0(cam, std::move(sc0), std::move(se0), 30.0f);

    scene0->setCameraController(std::move(c0));

    // scene 1
    Spline sc1;
    sc1.addCurve({
        glm::vec3(-10.0f, 20.0f, 40.0f),
        glm::vec3(-12.0f, 15.0f, 39.0f)
    });

    Spline se1;
    se1.addCurve({
        glm::vec3(0.0f, 30.0f, 0.0f),
        glm::vec3(0.0f, 25.0f, 0.0f)
    });

    CameraController c1(cam, std::move(sc1), std::move(se1), 3.0f);

    scene1->setCameraController(std::move(c1));

    // scene 2
    Spline sc2;
    sc2.addCurve({
        glm::vec3(-12.0f, 15.0f, 39.0f),
        glm::vec3(-30.0f, 10.0f, 30.0f),
        glm::vec3(-40.0f, 0.0f, 30.0f),
        glm::vec3(-20.0f, 3.0f, 20.0f)
    });

    Spline se2;
    se2.addCurve({
        glm::vec3(0.0f, 25.0f, 0.0f),
        glm::vec3(0.0f, -10.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f, 5.0f, 40.0f)
    });

    CameraController c2(cam, std::move(sc2), std::move(se2), 20.0f);

    scene2->setCameraController(std::move(c2));

    // scene 4
    Spline sc4;
    sc4.addCurve({
        glm::vec3(-20.0f, 3.0f, 20.0f),
        glm::vec3(-60.0f, 15.0f, 0.0f),
        glm::vec3(-30.0f, 20.0f, 0.0f),
        glm::vec3(3.0f, 5.0f, 0.0f)
    });

    Spline se4;
    se4.addCurve({
        glm::vec3(0.0f, 5.0f, 40.0f),
        glm::vec3(0.0f, 4.0f, 40.0f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    });

    CameraController c4(cam, std::move(sc4), std::move(se4), 20.0f);

    scene4->setCameraController(std::move(c4));

    // scene 5
    Spline sc5;
    sc5.addCurve({
        glm::vec3(3.0f, 5.0f, 0.0f),
        glm::vec3(4.0f, 2.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 10.0f),
        glm::vec3(0.0f, 3.0f, 20.0f)
    });

    Spline se5;
    se5.addCurve({
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.0f, 5.0f, 40.0f)
    });

    CameraController c5(cam, std::move(sc5), std::move(se5), 10.0f);

    scene5->setCameraController(std::move(c5));

    // scene 6
    Spline sc6;
    sc6.addCurve({
        glm::vec3(0.0f, 3.0f, 20.0f),
        glm::vec3(0.0f, 1.0f, 50.0f),
        glm::vec3(0.0f, 40.0f, 50.0f)
    });

    Spline se6;
    se6.addCurve({
        glm::vec3(0.0f, 5.0f, 40.0f),
        glm::vec3(0.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 5.0f, 0.0f)
    });

    CameraController c6(cam, std::move(sc6), std::move(se6), 20.0f);

    scene6->setCameraController(std::move(c6));
}

void MainApp::createMaterials() {
    ResourceManager::addMaterial({ glm::vec3(0.4f, 1.0f, 0.5f), 0.0f }, "ground");
    ResourceManager::addMaterial({ glm::vec3(8.0f, 8.0f, 15.0f), 0.0f }, "lightning");
    ResourceManager::addMaterial({ glm::vec3(10.0f), 1.0f }, "book");
}

void MainApp::createLights() {
    lightDir = glm::normalize(glm::vec3(-0.5f, 0.9f, -0.5f));

    // scene 0
    DirLight dir0;
    dir0.setDirection(lightDir);
    dir0.setColor(glm::vec3(0.5f));
    scene0->setDirLight(std::move(dir0));

    // scene 1
    DirLight dir1;
    dir1.setDirection(lightDir);
    dir1.setColor(glm::vec3(0.5f));
    scene1->setDirLight(std::move(dir1));

    PointLight p1;
    p1.setPosition(glm::vec3(0.0f, 12.5f, 5.0f));
    p1.setColor(glm::vec3(10.0f, 9.0f, 15.0f));
    scene1->addPointLight(std::move(p1));

    // scene 2
    DirLight dir2;
    dir2.setDirection(lightDir);
    dir2.setColor(glm::vec3(0.5f));
    scene2->setDirLight(std::move(dir2));

    PointLight p2;
    p2.setPosition(glm::vec3(-3.0f, 4.0f, 7.0f));
    p2.setColor(glm::vec3(3.0f, 1.0f, 0.0f));
    scene2->addPointLight(std::move(p2));

    PointLight p20;
    p20.setPosition(glm::vec3(-8.0f, 3.0f, 7.0f));
    p20.setColor(glm::vec3(2.6f, 0.9f, 0.0f));
    scene2->addPointLight(std::move(p20));

    PointLight p21;
    p21.setPosition(glm::vec3(2.0f, 3.0f, 7.0f));
    p21.setColor(glm::vec3(3.1f, 1.1f, 0.0f));
    scene2->addPointLight(std::move(p21));

    // scene 4
    DirLight dir4;
    dir4.setDirection(lightDir);
    dir4.setColor(glm::vec3(0.5f));
    scene4->setDirLight(std::move(dir4));

    PointLight p4;
    p4.setPosition(glm::vec3(0.5f, 0.5f, 0.5f));
    p4.setColor(glm::vec3(5.0f));
    scene4->addPointLight(std::move(p4));

    // scene 5
    DirLight dir5;
    dir5.setDirection(lightDir);
    dir5.setColor(glm::vec3(0.5f));
    scene5->setDirLight(std::move(dir5));

    PointLight p5;
    p5.setPosition(glm::vec3(0.5f, 0.5f, 0.5f));
    p5.setColor(glm::vec3(5.0f));
    scene5->addPointLight(std::move(p5));

    // scene 6
    DirLight dir6;
    dir6.setDirection(lightDir);
    dir6.setColor(glm::vec3(0.5f));
    scene6->setDirLight(std::move(dir6));
}

void MainApp::createRenderObjects() {
    // scene 0
    RenderObject house0;
    house0.setMesh("house");
    house0.setDiffuseTexture("house_diffuse");
    house0.setNormalTexture("house_normal");
    scene0->addRenderObject(std::move(house0), texturedGeomNormalsId);

    RenderObject ground0;
    ground0.setMesh("plane");
    ground0.setDiffuseTexture("grass");
    ground0.setScale(200.0f);
    scene0->addRenderObject(std::move(ground0), tiledGeomId);

    RenderObject happy0;
    happy0.setAnimationModel("happy_boy");
    happy0.setPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    happy0.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene0->addRenderObject(std::move(happy0), animatedId);

    // scene 1
    RenderObject house1;
    house1.setMesh("house");
    house1.setDiffuseTexture("house_diffuse");
    house1.setNormalTexture("house_normal");
    scene1->addRenderObject(std::move(house1), texturedGeomNormalsId);

    RenderObject ground1;
    ground1.setMesh("plane");
    ground1.setDiffuseTexture("grass");
    ground1.setScale(200.0f);
    scene1->addRenderObject(std::move(ground1), tiledGeomId);

    RenderObject sad0;
    sad0.setAnimationModel("sad_boy");
    sad0.setPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    sad0.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene1->addRenderObject(std::move(sad0), animatedId);

    RenderObject lightning0;
    lightning0.setMesh("lightning0");
    lightning0.setMaterial("lightning");
    lightningObjId = scene1->addRenderObject(std::move(lightning0), simpleGeomId);

    // scene 2
    RenderObject house2;
    house2.setMesh("house");
    house2.setDiffuseTexture("house_diffuse");
    house2.setNormalTexture("house_normal");
    scene2->addRenderObject(std::move(house2), texturedGeomNormalsId);

    RenderObject ground2;
    ground2.setMesh("plane");
    ground2.setDiffuseTexture("grass");
    ground2.setScale(200.0f);
    scene2->addRenderObject(std::move(ground2), tiledGeomId);

    RenderObject sad1;
    sad1.setAnimationModel("sad_boy");
    sad1.setPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    sad1.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene2->addRenderObject(std::move(sad1), animatedId);

    // scene 4
    RenderObject house4;
    house4.setMesh("ruin");
    house4.setDiffuseTexture("ruin_diffuse");
    house4.setNormalTexture("ruin_normal");
    house4.setRotation(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    house4.setScale(2.0f);
    scene4->addRenderObject(std::move(house4), texturedGeomNormalsId);

    RenderObject ground4;
    ground4.setMesh("plane");
    ground4.setDiffuseTexture("grass");
    ground4.setScale(200.0f);
    scene4->addRenderObject(std::move(ground4), tiledGeomId);

    RenderObject sad3;
    sad3.setAnimationModel("sad_boy");
    sad3.setPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    sad3.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene4->addRenderObject(std::move(sad3), animatedId);

    RenderObject book0;
    book0.setMesh("cube");
    book0.setDiffuseTexture("superbible");
    book0.setPosition(glm::vec3(1.0f, 0.2f, 0.5f));
    book0.setScale(1.0f);
    book0.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene4->addRenderObject(std::move(book0), texturedGeomId);

    // scene 5
    RenderObject house5;
    house5.setMesh("ruin");
    house5.setDiffuseTexture("ruin_diffuse");
    house5.setNormalTexture("ruin_normal");
    house5.setRotation(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    house5.setScale(2.0f);
    scene5->addRenderObject(std::move(house5), texturedGeomNormalsId);

    RenderObject ground5;
    ground5.setMesh("plane");
    ground5.setDiffuseTexture("grass");
    ground5.setScale(200.0f);
    scene5->addRenderObject(std::move(ground5), tiledGeomId);

    RenderObject happy5;
    happy5.setAnimationModel("happy_boy");
    happy5.setPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    happy5.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene5->addRenderObject(std::move(happy5), animatedId);

    RenderObject book1;
    book1.setMesh("cube");
    book1.setDiffuseTexture("superbible");
    book1.setPosition(glm::vec3(1.0f, 0.2f, 0.5f));
    book1.setScale(1.0f);
    book1.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene5->addRenderObject(std::move(book1), texturedGeomId);

    // scene 6
    RenderObject house6;
    house6.setMesh("house");
    house6.setDiffuseTexture("house_diffuse");
    house6.setNormalTexture("house_normal");
    scene6->addRenderObject(std::move(house6), texturedGeomNormalsId);

    RenderObject ground6;
    ground6.setMesh("plane");
    ground6.setDiffuseTexture("grass");
    ground6.setScale(200.0f);
    scene6->addRenderObject(std::move(ground6), tiledGeomId);

    RenderObject happy6;
    happy6.setAnimationModel("happy_boy");
    happy6.setPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    happy6.setRotation(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scene6->addRenderObject(std::move(happy6), animatedId);
}
