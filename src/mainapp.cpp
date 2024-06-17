#include "mainapp.hpp"
#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro");
    App::setVSync(true);
    fireshader.load("fireshader.vert", "fireshader.frag");
    fireshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());

    lightDir = glm::vec3(1.0f);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    particleSystem.init();
}

void MainApp::buildImGui() {
    // Placeholder for custom ImGui component
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Resolution: %d x %d", resolution.x, resolution.y);

    // Placeholder for custom ImGui component
    if (ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightDir), -1.0f, 1.0f)) {
        fireshader.set("uLightDir", lightDir);
    }

    ImGui::SliderFloat("t", &t, 0.0f, 1.0f);
}

void MainApp::render() {
    glm::vec3 pos = deCasteljau(spline, t);

    coolCamera.moveTo(pos);
    coolCamera.lookAt(glm::vec3(0.0f));

    if (coolCamera.updateIfChanged()) {
        fireshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fireshader.bind();
    mesh.draw();

    particleSystem.update(static_cast<float>(glfwGetTime()));
    particleSystem.render(coolCamera.projection() * coolCamera.view());
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action != Action::REPEAT) return;

    if (key == Key::W) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Direction);
    } else if (key == Key::S) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Direction);
    } else if (key == Key::A) {
        coolCamera.move(-delta * cameraSpeed * coolCamera.m_Right);
    } else if (key == Key::D) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Right);
    } else if (key == Key::SPACE) {
        coolCamera.move(delta * cameraSpeed * coolCamera.m_Up);
    } else if (key == Key::LEFT_SHIFT) {
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

glm::vec3 MainApp::deCasteljau(const std::vector<glm::vec3>& spline, float t) {
    std::vector<glm::vec3> points(spline.size());

    for (int i = 0; i < spline.size(); i++) {
        points[i] = glm::vec3(spline[i]);
    }

    int n = points.size();
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            points[i] = (1 - t) * points[i] + t * points[i + 1];
        }
    }

    return points[0];
}
