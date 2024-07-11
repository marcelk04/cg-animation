#include "mesh.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "common.hpp"
#include "config.hpp"
#include "objparser.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
using namespace glm;


////////////////////////// Manual mesh loading //////////////////////////

void Mesh::load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    // Load data into buffers
    numIndices = indices.size();
    vbo.load(Buffer::Type::ARRAY_BUFFER, vertices);
    ebo.load(Buffer::Type::INDEX_BUFFER, indices);

    // Bind buffers to VAO
    // TODO: Use DSA instead (but only OpenGL 4.5+, so not on macOS)
    vao.bind();
    vbo.bind(Buffer::Type::ARRAY_BUFFER);
    ebo.bind(Buffer::Type::INDEX_BUFFER);
    size_t stride = 3 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    vao.unbind();
}

void Mesh::load(const std::vector<VertexPCN>& vertices, const std::vector<unsigned int>& indices) {
    // Load data into buffers
    numIndices = indices.size();
    vbo.load(Buffer::Type::ARRAY_BUFFER, vertices);
    ebo.load(Buffer::Type::INDEX_BUFFER, indices);

    // Bind buffers to VAO
    // TODO: Use DSA instead (but only OpenGL 4.5+, so not on macOS)
    vao.bind();
    vbo.bind(Buffer::Type::ARRAY_BUFFER);
    ebo.bind(Buffer::Type::INDEX_BUFFER);
    // Vertex attributes
    size_t stride = sizeof(VertexPCN);
    // (location = 0) position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*) (0 * sizeof(float)));
    // (location = 1) uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*) (3 * sizeof(float)));
    // (location = 2) normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*) (5 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    vao.unbind();
}

void Mesh::load(const std::vector<VertexPCNT>& vertices, const std::vector<unsigned int>& indices) {
    // Load data into buffers
    numIndices = indices.size();
    vbo.load(Buffer::Type::ARRAY_BUFFER, vertices);
    ebo.load(Buffer::Type::INDEX_BUFFER, indices);

    // Bind buffers to VAO
    // TODO: Use DSA instead (but only OpenGL 4.5+, so not on macOS)
    vao.bind();
    vbo.bind(Buffer::Type::ARRAY_BUFFER);
    ebo.bind(Buffer::Type::INDEX_BUFFER);
    // Vertex attributes
    size_t stride = sizeof(VertexPCNT);
    // (location = 0) position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*) (0 * sizeof(float)));
    // (location = 1) uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*) (3 * sizeof(float)));
    // (location = 2) normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*) (5 * sizeof(float)));
    // (location = 3) tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*) (8 * sizeof(float)));

    glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, stride, (void*) (11 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*) (15 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    vao.unbind();
}

glm::mat4 Mesh::convertMatrixToGLMFormat(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
    to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
    to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
    to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;
    return to;
}

void Mesh::load(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    std::vector<VertexPCNT> vertices;
    std::vector<unsigned int> indices;
    std::map<std::string, std::string> nodeParentMap; // Add this line to declare the map for node-parent relationships

    // Check if animation exists
    if (scene->HasAnimations()) {
        std::cout << "Model has animation data." << std::endl;
        animationData.animation = scene->mAnimations[0];
        boneCount = 0;
    } else {
        std::cout << "Model does not have animation data." << std::endl;
    }

    int offset = 0;
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        std::cout << "Number of vertices, indices and bones: " << scene->mNumMeshes << " " << mesh->mNumVertices << " " << mesh->mNumBones << std::endl;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            VertexPCNT vertex;
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.texCoord = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
            vertex.tangent = mesh->mTangents ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z) : glm::vec3(0.0f, 0.0f, 0.0f);

            for (int j = 0; j < 4; ++j) {
                vertex.boneIDs[j] = -1;
                vertex.weights[j] = 0.0f;
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
            aiBone* bone = mesh->mBones[i];
            int boneIndex;
            if (boneInfoMap.find(bone->mName.data) == boneInfoMap.end()) {
                boneIndex = boneCount++;
                BoneInfo bi;
                bi.id = boneIndex;
                bi.BoneOffset = convertMatrixToGLMFormat(bone->mOffsetMatrix);
                boneInfoMap[bone->mName.data] = bi;
            } else {
                boneIndex = boneInfoMap[bone->mName.data].id;
            }

            for (unsigned int j = 0; j < bone->mNumWeights; ++j) {
                unsigned int vertexID = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;

                for (int k = 0; k < 4; ++k) {
                    if (vertices[vertexID].weights[k] == 0.0f) {
                        vertices[vertexID].boneIDs[k] = boneIndex;
                        vertices[vertexID].weights[k] = weight;
                        break;
                    }
                }
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(offset + face.mIndices[j]);
            }
        }

        offset += mesh->mNumVertices;
    }

    // Process the node hierarchy
    processNodeHierarchy(scene->mRootNode, scene, nodeParentMap);

    // Output node-parent relationships
    std::cout << "Node-Parent Relationships:" << std::endl;
    for (const auto& pair : nodeParentMap) {
        std::cout << "Node: " << pair.first << ", Parent: " << pair.second << std::endl;
    }

    // Output vertex bone influences
    std::cout << "Vertex Bone Influences:" << std::endl;
    for (const auto& vertex : vertices) {
        for (int j = 0; j < 4; ++j) {
            if (vertex.boneIDs[j] != -1) {
                std::cout << "  Bone ID: " << vertex.boneIDs[j] << ", Weight: " << vertex.weights[j] << std::endl;
            }
        }
    }

    load(vertices, indices);
}


void Mesh::loadWithTangents(const std::string& filepath) {
    std::vector<VertexPCNT> vertices;
    std::vector<unsigned int> indices;
    ObjParser::parse(filepath, vertices, indices);
    load(vertices, indices);
}

///////////////////////////// Mesh drawing /////////////////////////////

void Mesh::draw() {
    vao.bind();
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    vao.unbind();
}

void Mesh::processNodeHierarchy(aiNode* node, const aiScene* scene, std::map<std::string, std::string>& nodeParentMap) {
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        std::string childName(node->mChildren[i]->mName.data);
        std::string parentName(node->mName.data);
        nodeParentMap[childName] = parentName;
        processNodeHierarchy(node->mChildren[i], scene, nodeParentMap);
    }
}

