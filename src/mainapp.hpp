#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/mesh.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"

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
       Mesh mesh;
       Program meshshader;
       Camera boringCamera;

       glm::vec3 lightDir;
};