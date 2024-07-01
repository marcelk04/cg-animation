#include "renderobject.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderObject::RenderObject(Mesh& mesh)
	: m_Mesh(mesh), m_Material(std::nullopt) {
	setModelMatrix(glm::mat4(1.0f));
}

void RenderObject::setPosition(const glm::vec3& position) {
	setPositionAndSize(position, 1.0f);
}

void RenderObject::setPositionAndSize(const glm::vec3& position, const float scale) {
	setModelMatrix(glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(scale)));
}

void RenderObject::setModelMatrix(const glm::mat4& model) {
	m_Model = model;
	m_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
}

void RenderObject::setMaterial(const Material& material) {
	m_Material = std::make_optional<Material>(material);
}

void RenderObject::draw(Program& program) {
	program.bind();
	program.set("uLocalToWorld", m_Model);
	program.set("uNormalMatrix", m_NormalMatrix);

	if (m_Material.has_value()) {
		program.set("uMaterial.diffuse", m_Material->diffuse);
		program.set("uMaterial.specular", m_Material->specular);
	}

	m_Mesh.draw();
}
