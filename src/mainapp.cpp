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

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    texChecker.load(Texture::Format::SRGB8, "textures/checker.png", 0);
    texChecker.bind(Texture::Type::TEX2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textureshader.load("textureshader.vert", "textureshader.frag");
    textureshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    textureshader.bindTextureUnit("uTexture", 0);

    meshshader.load("meshshader.vert", "meshshader.frag");
    meshshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());

    plane.load("meshes/plane.obj");

    auto segments = LightningGenerator::genBolt(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5, coolCamera.m_Direction);
    auto meshdata = LightningGenerator::genMeshData(segments, coolCamera.m_Direction);

    lightningMesh.load(meshdata.first, meshdata.second);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Common::randomSeed();

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (ImGui::SphericalSlider("Light Direction", lightDir)) {
        textureshader.set("uLightDir", lightDir);
        meshshader.set("uLightDir", lightDir);
    }
}

void MainApp::render() {
    if (coolCamera.updateIfChanged()) {
        textureshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
        meshshader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texChecker.bind(Texture::Type::TEX2D);
    textureshader.bind();
    plane.draw();

    meshshader.bind();
    lightningMesh.draw();
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

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
    else if (key == Key::G) {
        auto segments = LightningGenerator::genBolt(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5, coolCamera.m_Direction);
        auto meshdata = LightningGenerator::genMeshData(segments, coolCamera.m_Direction);

        lightningMesh.load(meshdata.first, meshdata.second);
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
