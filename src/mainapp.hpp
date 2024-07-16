#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "lightninggenerator.hpp"
#include "resourcemanager.hpp"
#include "particlesystem.hpp"
#include "dark_animations/animation.hpp"
#include "dark_animations/animationmodel.hpp"
#include "dark_animations/animator.hpp"
#include "cinematic_engine/movingcamera.hpp"
#include "cinematic_engine/spline.hpp"
#include "cinematic_engine/cameracontroller.hpp"
#include "renderer/renderobject.hpp"
#include "renderer/renderer.hpp"
#include "renderer/light.hpp"

#include "framework/app.hpp"
#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"
#include "framework/gl/framebuffer.hpp"
#include "music.hpp"
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
    void scrollCallback(float amount) override;
    void moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) override;
    void resizeCallback(const vec2& resolution) override;
    void resetRenderTimer(float duration);

    void loadShaders();
    void loadObjects();
    void loadTextures();
    void initParticleSystem();
    void createMaterials();
    void createLights();
    void createRenderObjects();

private:
    std::shared_ptr<MovingCamera> cam;
    bool showControlPoints = false;

    Renderer renderer;
    uint32_t sceneIdx = 0;
    std::vector<std::shared_ptr<Scene>> scenes;

    std::shared_ptr<Scene> houseScene;
    std::shared_ptr<Scene> lightningScene;
    std::shared_ptr<Scene> sadScene;

    Material lightningMaterial;

    glm::vec3 lightDir;

    std::shared_ptr<Texture> tex;

    std::shared_ptr<Program> simpleGeom;
    size_t simpleGeomId;

    std::shared_ptr<Program> texturedGeomNormals;
    size_t texturedGeomNormalsId;

    std::shared_ptr<Program> texturedGeom;
    size_t texturedGeomId;

    std::shared_ptr<Program> animated;
    size_t animatedId;

    Animator animator;

    float elapsedTime;
    float renderDuration;

    SoundPlayer soundPlayer;
    bool soundPlayed;
};
