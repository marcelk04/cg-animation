#pragma once

#include "framework/mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>

#include <string>
#include <vector>
#include <iostream>

namespace Common {

    std::string absolutePath(const std::string& filepath);
    std::string readFile(const std::string& filepath);
    void writeToFile(const std::string& content, const std::string& filepath);
    void filesInDirectory(const std::string& directoryPath, const std::string& extension, std::vector<std::string>& filenames);

    template <class T, typename... Rest>
    void hash_combine(std::size_t& seed, const T& v, const Rest&... rest);


    glm::mat4 getGLMMat(const aiMatrix4x4& from);
    glm::vec3 getGLMVec(const aiVector3D& vec);
    glm::quat getGLMQuat(const aiQuaternion& pOrientation);

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
    os << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& vec) {
    os << "vec2(" << vec.x << ", " << vec.y << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Mesh::VertexPCNTB& vertex) {
    os << "VertexPCNTB: { position: " << vertex.position << ", texCoord: " << vertex.texCoord << ", normal: " << vertex.normal << ", tangent: " << vertex.tangent << ", boneIDs: [";
    for (int i = 0; i < 4; i++) {
        os << vertex.boneIDs[i] << ", ";
    }
    os << "], weights: [";
    for (int i = 0; i < 4; i++) {
        os << vertex.weights[i] << ", ";
    }
    os << "] }";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::mat4& mat) {
    os << "mat4(";
    for (int i = 0; i < 4; i++) {
        os << "[";
        for (int j = 0; j < 4; j++) {
            os << mat[i][j] << ", ";
        }
        os << "], ";
    }
    os << ")";
    return os;
}