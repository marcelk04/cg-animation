#pragma once

#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

struct BoneInfo {
    int id;
    glm::mat4 offset;
};

class AnimationModel {
public:
    AnimationModel() = default;
    AnimationModel(const std::string& path);

    void draw(Program& program);

    std::map<std::string, BoneInfo>& getBoneInfoMap() { return m_BoneInfoMap; }
    int& getBoneCount() { return m_BoneCounter; }

private:
    void loadModel(const std::string& path);

    void processNode(aiNode* node, const aiScene* scene);
    std::string processMesh(aiMesh* mesh, const aiScene* scene);

    void extractBoneWeightForVertices(std::vector<Mesh::VertexPCNTB>& vertices, aiMesh* mesh, const aiScene* scene);

    void setVertexBoneDataToDefault(Mesh::VertexPCNTB& vertex) const;
    void setVertexBoneData(Mesh::VertexPCNTB& vertex, int boneID, float weight) const;

private:
    std::vector<std::string> m_Meshes;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
};
