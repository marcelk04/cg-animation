#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "movingcamera.hpp"
#include "framework/app.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"
#include "mesh.hpp"
#include "dark_animations/animator.hpp"
#include "dark_animations/model_animation.hpp"

class MainApp : public App {
public:
    MainApp();
    ~MainApp();

protected:
    void init() override;
    void buildImGui() override;
    void render() override;
    void keyCallback(Key key, Action action) override;
    void scrollCallback(float amount) override;
    void moveCallback(const glm::vec2& movement, bool leftButton, bool rightButton, bool middleButton) override;

private:
    Mesh mesh;
    Program shaderProgram;
    MovingCamera coolCamera{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f) };
    std::vector<glm::vec3> spline{ glm::vec3(-2.0f, -1.0f, 3.0f), glm::vec3(0.0f, 3.0f, 2.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(3.0f, 1.5f, 0.0f) };
    float t = 0;
    glm::vec3 lightDir;
    Model model;
    Animator animator;
    Animation animation;
    std::vector<glm::mat4> previousTransforms;
    Mix_Music* music = nullptr;
};
