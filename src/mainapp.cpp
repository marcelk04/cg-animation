#include "mainapp.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "framework/app.hpp"

#include "framework/imguiutil.hpp"

#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

MainApp::MainApp() : App(800, 600) {
    App::setTitle("cgintro"); // set title
    App::setVSync(true); // Limit framerate

    lineShader.load("lineshader.vert", "lineshader.frag");
    lineShader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());

    lightDir = glm::vec3(1.0f);

    auto vertices = generateLightning(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 5);
    auto indices = generateLightningIndices(vertices.size());
    std::vector<float> verticesFloat(vertices.size() * 3);

    for (int i = 0; i < vertices.size(); i++) {
        std::cout << vertices[i] << std::endl;

        verticesFloat[3 * i + 0] = vertices[i].x;
        verticesFloat[3 * i + 1] = vertices[i].y;
        verticesFloat[3 * i + 2] = vertices[i].z;
    }

    for (int i : indices) {
        std::cout << i << " ";
    }

    std::cout << "\n";


    lightning.load(verticesFloat, indices);
}

void MainApp::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MainApp::buildImGui() {
    ImGui::StatisticsWindow(delta, resolution);

    if (ImGui::SphericalSlider("Light Direction", lightDir)) {
        lineShader.set("uLightDir", lightDir);
    }
}

void MainApp::render() {
    if (coolCamera.updateIfChanged()) {
        lineShader.set("uWorldToClip", coolCamera.projection() * coolCamera.view());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lineShader.bind();
    lightning.drawLines();
}

void MainApp::keyCallback(Key key, Action action) {
    float cameraSpeed = 2.5f;

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

void MainApp::moveCallback(const vec2& movement, bool leftButton, bool rightButton, bool middleButton) {
    if (leftButton || rightButton || middleButton) {
        coolCamera.rotate(0.002f * movement);
    }
}

glm::vec3 MainApp::deCasteljau(const std::vector<glm::vec3>& spline, float t) {
    std::vector<glm::vec3> points(spline.size());

    for (int i = 0; i < spline.size(); i++) {
        points[i] = glm::vec3(spline[i]);
    }

    int n = points.size();
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            points[i] = (1 - t) * points[i] + t * points[i + 1];
        }
    }

    return points[0];
}


std::vector<glm::vec3> MainApp::generateLightning(glm::vec3 start, glm::vec3 end, int generations) {
    std::vector<glm::vec3> pointList{start, end};

    float offsetAmount = 0.05f;

    for (int i = 0; i < generations; i++) {
        std::vector<glm::vec3> newPoints(2 * pointList.size() - 1);

        int k = 0;
        for (int j = 0; j < pointList.size()-1; j++) {
            glm::vec3& startPoint = pointList[j];
            glm::vec3& endPoint = pointList[j+1];

            glm::vec3 midPoint = 0.5f * (startPoint + endPoint);

            float randomValue = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            float randomOffset = 2 * randomValue * offsetAmount - offsetAmount;

            midPoint += randomOffset * glm::normalize(glm::cross(endPoint - startPoint, coolCamera.m_Direction));

            newPoints[k++] = startPoint;
            newPoints[k++] = midPoint;
        }

        newPoints[k] = pointList[pointList.size() - 1];

        pointList = newPoints;
    }

    return pointList;
}

std::vector<unsigned int> MainApp::generateLightningIndices(int n) {
    std::vector<unsigned int> indices(2* n - 2);
    
    unsigned int k = 0;
    for (int i = 0; i < indices.size(); i += 2) {
        indices[i] = k++;
        indices[i+1] = k;
    }

    return indices;
}
