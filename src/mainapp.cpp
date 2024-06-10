#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate
}