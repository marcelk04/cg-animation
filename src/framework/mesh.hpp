#pragma once

#include "gl/buffer.hpp"
#include "gl/vertexarray.hpp"

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <string>
#include <vector>
#include <map>

class Mesh {
public:
    struct VertexPCN {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
    };

    struct VertexPCNT {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
        glm::vec3 tangent;
    };

    struct VertexPCNTB {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
        glm::vec3 tangent;
        int boneIDs[4];
        float weights[4];
    };

    void load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void load(const std::vector<VertexPCN>& vertices, const std::vector<unsigned int>& indices);
    void load(const std::vector<VertexPCNT>& vertices, const std::vector<unsigned int>& indices);
    void load(const std::vector<VertexPCNTB>& vertices, const std::vector<unsigned int>& indices);
    void load(const std::string& filepath);
    void draw();

private:
    unsigned int numIndices = 0;
    VertexArray vao;
    Buffer vbo;
    Buffer ebo;
};
