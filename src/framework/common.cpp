#include "common.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "config.hpp"

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

int Common::randomInt(int first, int last) {
    return static_cast<int>(randomFloat(first, last));
}

float Common::randomFloat() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

float Common::randomFloat(float first, float last) {
    float interval = last - first;

    return randomFloat() * interval + first;
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