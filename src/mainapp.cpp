#include "mainapp.hpp"

#include "framework/imguiutil.hpp"
#include "framework/common.hpp"
#include "dark_animations/animator.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


MainApp::MainApp() : App(800, 600),  model(Common::absolutePath("rigged_model/dancing_vampire.dae")), animation(Common::absolutePath("rigged_model/dancing_vampire.dae"), &model), animator(&animation) {
    App::setTitle("cgintro"); // Set title
    App::setVSync(true); // Limit framerate

    // Load shaders
    shaderProgram.load("assimpshader.vert", "assimpshader.frag");
    shaderProgram.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    shaderProgram.set("view", coolCamera.view());
    shaderProgram.set("projection", coolCamera.projection());


    lightDir = glm::vec3(1.0f);
}


void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);
}

void MainApp::render() {
    if (coolCamera.updateIfChanged()) {
        shaderProgram.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    animator.updateAnimation(delta);

    shaderProgram.bind();
    
    // Set model matrix
    glm::mat4 modelMat = glm::mat4(1.0f);
//    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.0f, 0.0f));
//    modelMat = glm::rotate(modelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(0.5f));

    shaderProgram.set("uLocalToWorld", modelMat);

    // Update animation

    const auto& transforms = animator.getFinalBoneMatrices();
    
	for (int i = 0; i < transforms.size(); i++) {
		shaderProgram.set("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }

    model.draw(shaderProgram);
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 5.0f;

    if (action == Action::RELEASE) return;

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
