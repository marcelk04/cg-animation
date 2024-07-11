#include "mainapp.hpp"
#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "framework/imguiutil.hpp"

MainApp::MainApp() : App(800, 600), animator(nullptr) {
    App::setTitle("cgintro"); // Set title
    App::setVSync(true); // Limit framerate

    // Load the FBX mesh
    mesh.load("/home/timnogga/CLionProjects/cg-animation/rigged_model/surely.dae"); // Replace with your FBX model path

    // Load shaders
    shaderProgram.load("assimpshader.vert", "assimpshader.frag");
    shaderProgram.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    shaderProgram.set("view", coolCamera.view());
    shaderProgram.set("projection", coolCamera.projection());

    // Initialize the animator
    const aiScene* scene = mesh.getScene();
    if (scene->HasAnimations()) {
        animator = new Animator(scene->mAnimations[0], scene->mRootNode);
    }

    for (int i = 0; i < 100; i++) {
        shaderProgram.set("uBones[" + std::to_string(i) + "]", glm::mat4(1.0f));
    }

    lightDir = glm::vec3(1.0f);
}

MainApp::~MainApp() {
    delete animator;
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainApp::buildImGui() {
    // Custom ImGui statistics window
    ImGui::StatisticsWindow(delta, resolution);
}

void MainApp::render() {
    if (coolCamera.updateIfChanged()) {
        shaderProgram.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
        shaderProgram.set("view", coolCamera.view());
        shaderProgram.set("projection", coolCamera.projection());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    // Update animation
    if (animator) {
        animator->UpdateAnimation(delta);
        auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            shaderProgram.set("uBones[" + std::to_string(i) + "]", transforms[i]);
        }
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

    shaderProgram.set("model", model);
    mesh.draw();
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 10.0f;

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

/*
glm::vec3 MainApp::deCasteljau(const std::vector<glm::vec3>& spline, float t) {
    std::vector<glm::vec3> points(spline.size());

    for (int i = 0; i < spline.size(); i++) {
        points[i] = glm::vec3(spline[i]);
    }

    int n = points.size();
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            points[i] = (1 - t) * points[i] + t * points[i + 1]);
        }
    }

    return points[0];
}
*/
