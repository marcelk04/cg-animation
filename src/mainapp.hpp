#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "movingcamera.hpp"

#include "framework/app.hpp"

#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"
#include "particlesystem.hpp"
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
    MovingCamera coolCamera{ glm::vec3(0.0f, 1.0f, 50.0f), glm::vec3(0.0f) };

    Texture texture;
    Mesh plane;
    Program textureShader;

    glm::vec3 lightDir;
    Program fireshader;
    ParticleSystem particleSystem;
};

