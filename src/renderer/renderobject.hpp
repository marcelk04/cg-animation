#pragma once

#include "resourcemanager.hpp"
#include "cinematic_engine/movingcamera.hpp"
#include "dark_animations/animationmodel.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <optional>
#include <string>

struct Material {
	glm::vec3 diffuse;
	float specular;
};

class RenderObject {
public:
	RenderObject();

	void draw(Program& program);

	glm::mat4& getModelMatrix() { return m_Model; }
	std::optional<Material> getMaterial() const { return m_Material; }
	std::optional<std::string> getDiffuseTexture() { return m_DiffuseTexture; }
	std::optional<std::string> getNormalTexture() { return m_NormalTexture; }

	void setMesh(const std::string& meshname);
	void setAnimationModel(const std::string& modelname);
	void setPosition(const glm::vec3& position);
	void setScale(const float scale);
	void setRotation(const float angle, const glm::vec3& axis);
	void setRotation(const glm::quat& rotation);
	void setModelMatrix(const glm::mat4& model);
	void setMaterial(const Material& material);
	void setDiffuseTexture(const std::string& texturename);
	void setNormalTexture(const std::string& texturename);

private:
	void recalculateModelMatrix();

private:
	std::optional<std::string> m_Mesh;
	std::optional<std::string> m_AnimationModel;

	glm::vec3 m_Position;
	float m_Scale;
	glm::quat m_Rotation;

	std::optional<Material> m_Material;

	std::optional<std::string> m_DiffuseTexture;
	std::optional<std::string> m_NormalTexture;

	glm::mat4 m_Model;
	glm::mat3 m_NormalMatrix;
};