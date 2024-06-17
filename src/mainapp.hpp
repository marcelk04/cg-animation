#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "movingcamera.hpp"

#include "framework/app.hpp"

#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "lightninggenerator.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"

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
    MovingCamera coolCamera{ glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f) };

    Program textureshader;
    Program meshshader;
    Mesh lightningMesh;
    Mesh plane;
    Texture texChecker;

    glm::vec3 lightDir{ glm::vec3(1.0f) };
};