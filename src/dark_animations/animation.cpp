#include "animation.hpp"

#include "framework/common.hpp"

Animation::Animation(const std::string& animationPath, AnimationModel* model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(Common::absolutePath(animationPath), aiProcess_Triangulate);

    assert(scene && scene->mRootNode);

    aiAnimation* animation = scene->mAnimations[0];

    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;

    aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;

    globalTransformation = globalTransformation.Inverse();

    readHierarchyData(m_RootNode, scene->mRootNode);
    readMissingBones(animation, *model);
}

Bone* Animation::findBone(const std::string& name) {
    for (Bone& bone : m_Bones) {
        if (bone.getBoneName() == name) {
            return &bone;
        }
    }

    return nullptr;
}

void Animation::readMissingBones(const aiAnimation* animation, AnimationModel& model) {
    int size = animation->mNumChannels;

    auto& boneInfoMap = model.getBoneInfoMap();
    int& boneCount = model.getBoneCount();

    //reading channels (bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++) {
        const aiNodeAnim* channel = animation->mChannels[i];
        const std::string& boneName = channel->mNodeName.data;

        // if bone doesn't exist yet
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }

        m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_BoneInfoMap = boneInfoMap;
}

void Animation::readHierarchyData(AssimpNodeData &dest, const aiNode *src) const {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = Common::getGLMMat(src->mTransformation);

    for (int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;

        readHierarchyData(newData, src->mChildren[i]);

        dest.children.push_back(newData);
    }
}
