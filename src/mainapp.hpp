#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "movingcamera.hpp"
#include "renderer/renderobject.hpp"
#include "renderer/renderer.hpp"
#include "renderer/light.hpp"

#include "framework/app.hpp"

#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"
#include "framework/gl/framebuffer.hpp"

#include <vector>
#include <memory>

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
    void resizeCallback(const vec2& resolution) override;

private:
    std::shared_ptr<MovingCamera> cam;

    Renderer renderer;

    Mesh cube;

    std::shared_ptr<Program> meshshader;
    std::shared_ptr<Program> lightingshader;
    std::shared_ptr<Program> colorshader;

    std::vector<glm::vec3> lightPositions{ glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f), glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f) };
};