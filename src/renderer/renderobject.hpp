#pragma once

#include "cinematic_engine/movingcamera.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <optional>

struct Material {
	glm::vec3 diffuse;
	float specular;
};

class RenderObject {
public:
	RenderObject(Mesh& mesh);

	void draw(Program& program);

	glm::mat4& getModelMatrix() { return m_Model; }
	std::optional<Material> getMaterial() const { return m_Material; }
	std::shared_ptr<Texture> getDiffuseTexture() { return m_DiffuseTexture; }
	std::shared_ptr<Texture> getNormalTexture() { return m_NormalTexture; }

	void setPosition(const glm::vec3& position);
	void setScale(const float scale);
	void setRotation(const float angle, const glm::vec3& axis);
	void setRotation(const glm::quat& rotation);
	void setModelMatrix(const glm::mat4& model);
	void setMaterial(const Material& material);
	void setDiffuseTexture(std::shared_ptr<Texture> diffuseTexture);
	void setNormalTexture(std::shared_ptr<Texture> normalTexture);

private:
	void recalculateModelMatrix();

private:
	Mesh& m_Mesh;

	glm::vec3 m_Position;
	float m_Scale;
	glm::quat m_Rotation;

	std::optional<Material> m_Material;

	std::shared_ptr<Texture> m_DiffuseTexture;
	std::shared_ptr<Texture> m_NormalTexture;

	glm::mat4 m_Model;
	glm::mat3 m_NormalMatrix;
};