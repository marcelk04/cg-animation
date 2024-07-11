/*
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <assimp/scene.h>
#include "gl/buffer.hpp"
#include "gl/vertexarray.hpp"



class Mesh {
public:
    struct VertexPCN {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
        int boneIDs[4];
        float weights[4];
    };

    struct VertexPCNT {
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
    void load(const std::string& filepath);
    void loadWithTangents(const std::string& filepath);
    void draw();
    const aiScene* getScene() const { return scene; }

private:
    void processNodeHierarchy(aiNode* node, const aiScene* scene, std::map<std::string, std::string>& nodeParentMap);

    glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from);

    unsigned int numIndices = 0;
    VertexArray vao;
    Buffer vbo;
    Buffer ebo;

    std::map<std::string, BoneInfo> boneInfoMap;
    int boneCount = 0;

    const aiScene* scene;

    struct AnimationData {
        const aiAnimation* animation;
        float currentTime = 0.0f;
    };
    AnimationData animationData;
};
*/
