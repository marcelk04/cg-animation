#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "movingcamera.hpp"

#include "framework/app.hpp"

#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"

#include <vector>

class MainApp : public App {
public:
    MainApp();

protected:
    void init() override;
    void buildImGui() override;
    void render() override;
    void keyCallback(Key key, Action action) override;
    // void clickCallback(Button button, Action action, Modifier modifier) override;
    void scrollCallback(float amount) override;
    void moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) override;
    // void resizeCallback(const vec2& resolution) override;

private:
    glm::vec3 deCasteljau(const std::vector<glm::vec3>& spline, float t);

private:
    Mesh mesh;
    Program meshshader;
    MovingCamera coolCamera{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f) };
    //std::vector<glm::vec3> spline{ glm::vec3(-2.0f, -1.0f, 3.0f), glm::vec3(-1.0f, -1.0f, 3.0f), glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3(2.0f, 1.0f, 3.0f)};
    //std::vector<glm::vec3> spline{ glm::vec3(-2.0f, -1.0f, 3.0f), glm::vec3(0.0f, 3.0f, 2.0f), glm::vec3(2.0f, -1.0f, 3.0f) };
    std::vector<glm::vec3> spline{ glm::vec3(-2.0f, -1.0f, 3.0f), glm::vec3(0.0f, 3.0f, 2.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(3.0f, 1.5f, 0.0f) };
    float t = 0;

    glm::vec3 lightDir;
};