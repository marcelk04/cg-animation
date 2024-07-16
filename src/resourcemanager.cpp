#include "resourcemanager.hpp"

#include "framework/common.hpp"

void ResourceManager::loadTexture(const std::string& filepath, const std::string& name) {
	Texture texture;
	texture.load(Texture::Format::SRGB8, Common::absolutePath(filepath), 0);

	addTexture(std::move(texture), name);
}

void ResourceManager::addTexture(Texture&& texture, const std::string& name) {
	s_Textures[name] = std::move(texture);
}

Texture& ResourceManager::getTexture(const std::string& name) {
	return s_Textures[name];
}

void ResourceManager::loadMesh(const std::string& filepath, const std::string& name) {
	Mesh mesh;
	mesh.load(Common::absolutePath(filepath));

	addMesh(std::move(mesh), name);
}

void ResourceManager::addMesh(Mesh&& mesh, const std::string& name) {
	s_Meshes[name] = std::move(mesh);
}

Mesh& ResourceManager::getMesh(const std::string& name) {
	return s_Meshes[name];
}

void ResourceManager::loadAnimationModel(const std::string& filepath, const std::string& name) {
	AnimationModel model(Common::absolutePath(filepath));

	addAnimationModel(std::move(model), name);
}

void ResourceManager::addAnimationModel(AnimationModel&& animationModel, const std::string& name) {
	s_AnimationModels[name] = std::move(animationModel);
}

AnimationModel& ResourceManager::getAnimationModel(const std::string& name) {
	return s_AnimationModels[name];
}

void ResourceManager::loadAnimation(const std::string& filepath, const std::string& modelname, const std::string& name) {
	Animation animation(Common::absolutePath(filepath), getAnimationModel(modelname));

	addAnimation(std::move(animation), name);
}

void ResourceManager::addAnimation(Animation&& animation, const std::string& name) {
	s_Animations[name] = std::move(animation);
}

Animation& ResourceManager::getAnimation(const std::string& name) {
	return s_Animations[name];
}

std::unordered_map<std::string, Texture> ResourceManager::s_Textures;
std::unordered_map<std::string, Mesh> ResourceManager::s_Meshes;
std::unordered_map<std::string, AnimationModel> ResourceManager::s_AnimationModels;
std::unordered_map<std::string, Animation> ResourceManager::s_Animations;