#pragma once

#include "movingcamera.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <optional>

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class RenderObject {
public:
	RenderObject(Mesh& mesh);

	glm::mat4& getModelMatrix() { return m_Model; }
	std::optional<Material> getMaterial() { return m_Material; }
	std::optional<glm::vec3> getColor() { return m_Color; }

	void setPosition(const glm::vec3& position);
	void setPositionAndSize(const glm::vec3& position, const float scale);
	void setModelMatrix(const glm::mat4& model);
	void setMaterial(const Material& material);
	void setColor(const glm::vec3& color);

	void draw(Program& program, MovingCamera& cam);

private:
	Mesh& m_Mesh;

	std::optional<Material> m_Material;
	std::optional<glm::vec3> m_Color;

	glm::mat4 m_Model;
	glm::mat3 m_NormalMatrix;
};