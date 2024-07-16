#pragma once

#include "dark_animations/animationmodel.hpp"
#include "dark_animations/animation.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/texture.hpp"
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

	static void loadAnimationModel(const std::string& filepath, const std::string& name);
	static void addAnimationModel(AnimationModel&& animationModel, const std::string& name);
	static AnimationModel& getAnimationModel(const std::string& name);

	static void loadAnimation(const std::string& filepath, const std::string& modelname, const std::string& name);
	static void addAnimation(Animation&& animation, const std::string& name);
	static Animation& getAnimation(const std::string& name);

private:
	static std::unordered_map<std::string, Texture> s_Textures;
	static std::unordered_map<std::string, Mesh> s_Meshes;
	static std::unordered_map<std::string, AnimationModel> s_AnimationModels;
	static std::unordered_map<std::string, Animation> s_Animations;
};