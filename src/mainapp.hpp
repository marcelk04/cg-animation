#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "cinematic_engine/movingcamera.hpp"
#include "cinematic_engine/spline.hpp"
#include "renderer/renderobject.hpp"
#include "renderer/renderer.hpp"
#include "renderer/light.hpp"

#include "framework/app.hpp"

#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "lightninggenerator.hpp"
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
    Scene scene;

    Mesh cube;
    Mesh plane;
    Mesh sphere;
    Mesh bunny;
    Mesh house;

    glm::vec3 lightDir;

    std::shared_ptr<Program> simpleGeom;
    size_t simpleGeomId;

    std::shared_ptr<Program> texturedGeom;
    size_t texturedGeomId;
};