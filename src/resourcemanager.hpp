#pragma once

#include "framework/gl/texture.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/shader.hpp"

#include <vector>
#include <unordered_map>
#include <string>

class ResourceManager {
public:
	static void loadTexture(const std::string& filepath, const std::string& name);
	static void addTexture(Texture&& texture, const std::string& name);
	static Texture& getTexture(const std::string& name);

	static void loadMesh(const std::string& filepath, const std::string& name);
	static void addMesh(Mesh&& mesh, const std::string& name);
	static Mesh& getMesh(const std::string& name);

private:
	static std::unordered_map<std::string, Texture> s_Textures;
	static std::unordered_map<std::string, Mesh> s_Meshes;
};