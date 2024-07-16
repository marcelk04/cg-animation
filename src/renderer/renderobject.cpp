#include "renderobject.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

RenderObject::RenderObject()
	: m_Mesh(std::nullopt),
	  m_AnimationModel(std::nullopt),
	  m_Position(glm::vec3(0.0f)),
	  m_Scale(1.0f),
	  m_Rotation(glm::angleAxis(0.0f, glm::vec3(1.0f))),
	  m_DiffuseTexture(std::nullopt),
	  m_NormalTexture(std::nullopt) {
	setModelMatrix(glm::mat4(1.0f));
}

void RenderObject::draw(Program& program) {
	program.bind();
	program.set("uLocalToWorld", m_Model);
	program.set("uNormalMatrix", m_NormalMatrix);

	if (m_Material.has_value()) {
		program.set("uMaterial.diffuse", m_Material->diffuse);
		program.set("uMaterial.specular", m_Material->specular);
	}

	if (m_DiffuseTexture.has_value()) {
		ResourceManager::getTexture(m_DiffuseTexture.value()).bind(Texture::Type::TEX2D, 0);
	}

	if (m_NormalTexture.has_value()) {
		ResourceManager::getTexture(m_NormalTexture.value()).bind(Texture::Type::TEX2D, 1);
	}

	if (m_Mesh.has_value()) {
		ResourceManager::getMesh(*m_Mesh).draw();
	}

	if (m_AnimationModel.has_value()) {
		ResourceManager::getAnimationModel(*m_AnimationModel).draw(program);
	}
}

void RenderObject::setMesh(const std::string& meshname) {
	m_Mesh = std::make_optional<std::string>(meshname);
}

void RenderObject::setAnimationModel(const std::string& modelname) {
	m_AnimationModel = std::make_optional<std::string>(modelname);
}

void RenderObject::setPosition(const glm::vec3& position) {
	m_Position = position;

	recalculateModelMatrix();
}

void RenderObject::setScale(const float scale) {
	m_Scale = scale;

	recalculateModelMatrix();
}

void RenderObject::setRotation(const float angle, const glm::vec3& axis) {
	m_Rotation = glm::angleAxis(angle, axis);

	recalculateModelMatrix();
}

void RenderObject::setRotation(const glm::quat& rotation) {
	m_Rotation = rotation;

	recalculateModelMatrix();
}

void RenderObject::setModelMatrix(const glm::mat4& model) {
	m_Model = model;
	m_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
}

void RenderObject::setMaterial(const Material& material) {
	m_Material = std::make_optional<Material>(material);
}

void RenderObject::setDiffuseTexture(const std::string& texturename) {
	m_DiffuseTexture = std::make_optional<std::string>(texturename);
}

void RenderObject::setNormalTexture(const std::string& texturename) {
	m_NormalTexture = std::make_optional<std::string>(texturename);
}

void RenderObject::recalculateModelMatrix() {
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_Scale));
	glm::mat4 rotate = glm::mat4_cast(m_Rotation);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_Position);

	setModelMatrix(translate * rotate * scale);
}