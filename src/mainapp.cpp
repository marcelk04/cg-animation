#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/imguiutil.hpp"

#include <iostream>
#include <memory>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

MainApp::MainApp()
    : App(800, 600), cam(std::make_shared<MovingCamera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f))), renderer(cam) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    cam->setResolution(resolution);

    lightingshader = std::make_shared<Program>();
    lightingshader->load("lightingshader.vert", "lightingshader.frag");
    size_t lightingshaderId = renderer.addProgram(lightingshader);

    meshshader = std::make_shared<Program>();
    meshshader->load("meshshader.vert", "meshshader.frag");
    size_t meshshaderId = renderer.addProgram(meshshader);

    colorshader = std::make_shared<Program>();
    colorshader->load("colorshader.vert", "colorshader.frag");
    size_t colorshaderId = renderer.addProgram(colorshader);

    hdrshader = std::make_shared<Program>();
    hdrshader->load("hdrshader.vert", "hdrshader.frag");
    hdrshader->bindTextureUnit("uHdrBuffer", 0);

    cube.load("meshes/cube.obj");

    Material material{
        glm::vec3(1.0f, 0.5f, 0.31f),
        glm::vec3(1.0f, 0.5f, 0.31f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        64.0f
    };

    RenderObject normalCube(cube);
    normalCube.setPositionAndSize(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f);
    normalCube.setMaterial(material);
    renderer.addObject(std::move(normalCube), lightingshaderId);

    DirLight dirLight;
    dirLight.direction = glm::vec3(0.1f, 1.0f, 0.5f);
    dirLight.ambient = glm::vec3(0.2f);
    dirLight.diffuse = glm::vec3(0.5f);
    dirLight.specular = glm::vec3(1.0f);

    renderer.setDirLight(std::move(dirLight));

    for (size_t i = 0; i < 4; i++) {
        PointLight pointLight;
        pointLight.position = lightPositions[i];
        pointLight.ambient = glm::vec3(0.2f);
        pointLight.diffuse = glm::vec3(0.5f);
        pointLight.specular = glm::vec3(1.0f);
        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        renderer.setPointLight(std::move(pointLight), i);

        RenderObject lightCube(cube);
        lightCube.setPositionAndSize(lightPositions[i], 0.2f);
        lightCube.setColor(glm::vec3(1.0f));
        renderer.addObject(std::move(lightCube), colorshaderId);
    }

    float factor = 10.0f;
    PointLight& pl = renderer.getPointLights()[3];
    //pl.ambient *= factor;
    pl.diffuse *= factor;
    pl.specular *= factor;

    renderer.updateLightingUniforms();
    renderer.updateCamUniforms();

    colorBuffer.bind(Texture::Type::TEX2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    depthBuffer.bind(Texture::Type::TEX2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    hdrBuffer.attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::COLOR0, colorBuffer.handle);
    hdrBuffer.attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::DEPTH_STENCIL, depthBuffer.handle);

    quad.load(vertices, indices);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);
}

void MainApp::render() {
    if (cam->updateIfChanged()) {
        renderer.updateCamUniforms();
    }

    // Geometry pass
    hdrBuffer.bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer.draw();

    // HDR pass
    Framebuffer::bindDefault();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    colorBuffer.bind(Texture::Type::TEX2D, 0);
    hdrshader->bind();
    quad.draw();
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

    if (action != Action::REPEAT) return;

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
}

