#include "resourcemanager.hpp"

void ResourceManager::loadTexture(const std::string& filepath, const std::string& name) {
	Texture texture;
	texture.load(Texture::Format::SRGB8, filepath, 0);

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
	mesh.load(filepath);

	addMesh(std::move(mesh), name);
}

void ResourceManager::addMesh(Mesh&& mesh, const std::string& name) {
	s_Meshes[name] = std::move(mesh);
}

void ResourceManager::loadMeshWithTangents(const std::string& filepath, const std::string& name) {
	Mesh mesh;
	mesh.loadWithTangents(filepath);

	s_Meshes[name] = std::move(mesh);
}

Mesh& ResourceManager::getMesh(const std::string& name) {
	return s_Meshes[name];
}

std::unordered_map<std::string, Texture> ResourceManager::s_Textures;
std::unordered_map<std::string, Mesh> ResourceManager::s_Meshes;