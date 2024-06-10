#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/imguiutil.hpp"

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    mesh.load("meshes/bunny.obj");
    meshshader.load("meshshader.vert", "meshshader.frag");

    lightDir = glm::vec3(1.0f);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (ImGui::SphericalSlider("Light Direction", lightDir)) {
        meshshader.set("uLightDir", lightDir);
    }
}

void MainApp::render() {
    if (boringCamera.updateIfChanged()) {
        meshshader.set("uWorldToClip", boringCamera.projectionMatrix * boringCamera.viewMatrix);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    meshshader.bind();
    mesh.draw();
}

void MainApp::keyCallback(Key key, Action action) {

}

void MainApp::scrollCallback(float amount) {
    boringCamera.zoom(amount);
}

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) {
        boringCamera.rotate(0.01f * movement);
    }
}
