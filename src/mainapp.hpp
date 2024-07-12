#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "movingcamera.hpp"
#include "framework/app.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"
#include "mesh.hpp"
#include "animator.hpp"  // Include Animator class

using namespace glm;

class MainApp : public App {
public:
    MainApp();

protected:
    void init() override;
    void buildImGui() override;
    void render() override;
    void keyCallback(Key key, Action action) override;
    void scrollCallback(float amount) override;
    void moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) override;

private:
    //glm::vec3 deCasteljau(const std::vector<glm::vec3>& spline, float t);

private:
    Mesh mesh;
    Program shaderProgram;
    MovingCamera coolCamera{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f) };
    std::vector<glm::vec3> spline{ glm::vec3(-2.0f, -1.0f, 3.0f), glm::vec3(0.0f, 3.0f, 2.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(3.0f, 1.5f, 0.0f) };
    float t = 0;
    glm::vec3 lightDir;

    std::shared_ptr<Animator> animator;  // Animator instance
};
