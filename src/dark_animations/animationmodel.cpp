#include "dark_animations/animationmodel.hpp"

#include "resourcemanager.hpp"
#include "framework/common.hpp"

AnimationModel::AnimationModel(const std::string &path) {
    loadModel(Common::absolutePath(path));
}

void AnimationModel::draw(Program &program) {
    program.bind();

    for (const auto& meshName : m_Meshes) {
        ResourceManager::getMesh(meshName).draw();
    }
}

void AnimationModel::loadModel(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void AnimationModel::processNode(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        m_Meshes.push_back(processMesh(mesh, scene));
    }

    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

std::string AnimationModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Mesh::VertexPCNTB> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Mesh::VertexPCNTB vertex;

        setVertexBoneDataToDefault(vertex);

        vertex.position = Common::getGLMVec(mesh->mVertices[i]);
        vertex.normal = Common::getGLMVec(mesh->mNormals[i]);

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec;
        } else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (uint32_t j = 0; j < face.mNumIndices; j++) {
           indices.push_back(face.mIndices[j]);
        }
    }

    //aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    extractBoneWeightForVertices(vertices, mesh, scene);

    Mesh meshObj;
    meshObj.load(vertices, indices);

    std::string id = mesh->mName.C_Str();

    ResourceManager::addMesh(std::move(meshObj), id);

    return id;
}

void AnimationModel::extractBoneWeightForVertices(std::vector<Mesh::VertexPCNTB> &vertices, aiMesh *mesh, const aiScene *scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            boneID = m_BoneCounter;

            BoneInfo newBoneInfo;
            newBoneInfo.id = boneID;
            newBoneInfo.offset = Common::getGLMMat(mesh->mBones[boneIndex]->mOffsetMatrix);

            m_BoneInfoMap[boneName] = newBoneInfo;

            m_BoneCounter++;
        } else {
            boneID = m_BoneInfoMap[boneName].id;
        }

        assert(boneID != -1);

        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        uint32_t numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (uint32_t weightIndex = 0; weightIndex < numWeights; weightIndex++) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;

            assert(vertexId <= vertices.size());

            setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

void AnimationModel::setVertexBoneDataToDefault(Mesh::VertexPCNTB& vertex) const {
    for (int i = 0; i < 4; i++) {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void AnimationModel::setVertexBoneData(Mesh::VertexPCNTB& vertex, int boneID, float weight) const {
    for (int i = 0; i < 4; i++) {
        if (vertex.boneIDs[i] < 0) {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;

            break;
        }
    }
}
