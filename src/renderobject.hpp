#pragma once

#include "movingcamera.hpp"
#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"

#include <glm/glm.hpp>

#include <memory>

class RenderObject {
public:
	RenderObject(Mesh& mesh, const glm::vec3& color);
	RenderObject(Mesh& mesh, const glm::vec3& color, const glm::vec3& position, const float scale = 1.0f);
	RenderObject(Mesh& mesh, const glm::vec3& color, const glm::mat4& model);

	glm::vec3& getColor() { return m_Color; }
	glm::mat4& getModelMatrix() { return m_Model; }

	void draw(Program& program, MovingCamera& cam);

private:
	Mesh& m_Mesh;

	glm::vec3 m_Color;
	glm::mat4 m_Model;
	glm::mat3 m_NormalMatrix;
};