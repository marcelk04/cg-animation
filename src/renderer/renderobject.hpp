#pragma once

#include "movingcamera.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <optional>

struct Material {
	glm::vec3 diffuse;
	float specular;
};

class RenderObject {
public:
	RenderObject(Mesh& mesh);

	glm::mat4& getModelMatrix() { return m_Model; }
	std::optional<Material> getMaterial() { return m_Material; }

	void setPosition(const glm::vec3& position);
	void setPositionAndSize(const glm::vec3& position, const float scale);
	void setModelMatrix(const glm::mat4& model);
	void setMaterial(const Material& material);

	void draw(Program& program);

private:
	Mesh& m_Mesh;

	std::optional<Material> m_Material;

	glm::mat4 m_Model;
	glm::mat3 m_NormalMatrix;
};