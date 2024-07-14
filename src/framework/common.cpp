#include "common.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "config.hpp"

std::string Common::absolutePath(const std::string& filepath) {
    Config::setWorkingDirectory();
    std::filesystem::path path(filepath);
    return std::filesystem::absolute(path);
}

std::string Common::readFile(const std::string& filepath) {
    Config::setWorkingDirectory();
    std::filesystem::path path{filepath};
    std::ifstream stream{path};
    std::cout << "Loading " << std::filesystem::absolute(path) << std::endl;
    if (!stream.is_open()) throw std::runtime_error("Could not open file: " + std::filesystem::absolute(path).string());
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

void Common::writeToFile(const std::string& content, const std::string& filepath) {
    Config::setWorkingDirectory();
    std::filesystem::path path{filepath};
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out{path};
    std::cout << "Writing " << std::filesystem::absolute(path) << std::endl;
    if (!out.is_open()) throw std::runtime_error("Could not open file: " + std::filesystem::absolute(path).string());
    out << content;
    out.close();
}

void Common::filesInDirectory(const std::string& directoryPath, const std::string& extension, std::vector<std::string>& filenames) {
    Config::setWorkingDirectory();
    if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == extension) {
                filenames.push_back(entry.path().relative_path().string());
            }
        }
    }
}

void Common::randomSeed() {
    std::srand(std::time(0));
}

int Common::randomInt(int min, int max) {
    return static_cast<int>(randomFloat(min, max));
}

float Common::randomFloat() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

float Common::randomFloat(float min, float max) {
    return randomFloat() * (max - min) + min;
}

glm::vec3 Common::deCasteljau(std::vector<glm::vec3> points, float t) {
    int n = points.size();
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            points[i] = (1 - t) * points[i] + t * points[i + 1];
        }
    }

    return points[0];
}

glm::mat4 Common::getGLMMat(const aiMatrix4x4 &from) {
     glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

glm::vec3 Common::getGLMVec(const aiVector3D &vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat Common::getGLMQuat(const aiQuaternion &pOrientation) {
    return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}