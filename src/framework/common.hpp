#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include <glm/glm.hpp>

namespace Common {

    std::string readFile(const std::string& filepath);
    void writeToFile(const std::string& content, const std::string& filepath);
    void filesInDirectory(const std::string& directoryPath, const std::string& extension, std::vector<std::string>& filenames);

    template <class T, typename... Rest>
    void hash_combine(std::size_t& seed, const T& v, const Rest&... rest);

    void randomSeed();
    int randomInt(int min, int max);
    float randomFloat();
    float randomFloat(float min, float max);

    glm::vec3 deCasteljau(std::vector<glm::vec3> points, float t);
}

/**
 * Hash combine function from boost
 * Source: https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
 */
template <class T, typename... Rest>
inline void Common::hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    (hash_combine(seed, rest), ...);
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}