#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "cinematic_engine/movingcamera.hpp"
#include "cinematic_engine/spline.hpp"
#include "cinematic_engine/cameracontroller.hpp"
#include "renderer/renderobject.hpp"
#include "renderer/renderer.hpp"
#include "renderer/light.hpp"
#include "lightninggenerator.hpp"
#include "resourcemanager.hpp"

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

    void loadShaders();
    void loadObjects();
    void loadTextures();
    void createMaterials();
    void createLights();
    void createRenderObjects();

private:
    std::shared_ptr<MovingCamera> cam;
    bool showControlPoints = false;

    Renderer renderer;
    std::shared_ptr<Scene> scene;

    Material lightMaterial;
    Material floorMaterial;
    Material leftWallMaterial;
    Material backWallMaterial;
    Material rightWallMaterial;
    Material bunnyMaterial;

    glm::vec3 lightDir;

    std::shared_ptr<Texture> tex;

    std::shared_ptr<Program> simpleGeom;
    size_t simpleGeomId;

    std::shared_ptr<Program> texturedGeomNormals;
    size_t texturedGeomNormalsId;

    std::shared_ptr<Program> texturedGeom;
    size_t texturedGeomId;
};