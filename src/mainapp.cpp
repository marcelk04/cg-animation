#include "mainapp.hpp"
#include "framework/imguiutil.hpp"
#include "framework/common.hpp"
#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

MainApp::MainApp() : App(800, 600), model(Common::absolutePath("rigged_model/ultrarigged.dae")), animation(Common::absolutePath("rigged_model/ultrarigged.dae"), &model), animator(&animation) {
    App::setTitle("cgintro"); // Set title
    App::setVSync(true); // Limit framerate

    // Load shaders
    shaderProgram.load("assimpshader.vert", "assimpshader.frag");
    shaderProgram.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    shaderProgram.set("view", coolCamera.view());
    shaderProgram.set("projection", coolCamera.projection());

    lightDir = glm::vec3(1.0f);
}

MainApp::~MainApp() {
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Initialize SDL for audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Load music
    music = Mix_LoadMUS(Common::absolutePath("/home/timnogga/Downloads/rick.mp3").c_str());
    if (!music) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }

    // Play music
    if (Mix_PlayMusic(music, -1) == -1) {
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }
}

void MainApp::buildImGui() {
    // Custom ImGui statistics window
    ImGui::StatisticsWindow(delta, resolution);
}

void MainApp::render() {
    if (coolCamera.updateIfChanged()) {
        shaderProgram.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
        shaderProgram.set("view", coolCamera.view());
        shaderProgram.set("projection", coolCamera.projection());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    animator.UpdateAnimation(delta);

    shaderProgram.bind();

    // Set model matrix
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(0.2f, 0.2f, 0.2f));

    shaderProgram.set("model", modelMat);

    auto transforms = animator.GetFinalBoneMatrices();

    // Ensure the previousTransforms vector is the same size as transforms
    if (previousTransforms.size() != transforms.size()) {
        previousTransforms.resize(transforms.size(), glm::mat4(1.0f));
    }

    for (int i = 0; i < transforms.size(); ++i) {
        if (previousTransforms[i] != transforms[i]) {
            previousTransforms[i] = transforms[i];
        }
        shaderProgram.set("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }

    model.Draw(shaderProgram);
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 50.0f;

    if (action != Action::REPEAT) return;

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
}

void MainApp::scrollCallback(float amount) {
    coolCamera.zoom(0.1f * amount);
}

void MainApp::moveCallback(const glm::vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) {
        coolCamera.rotate(0.002f * movement);
    }
}
