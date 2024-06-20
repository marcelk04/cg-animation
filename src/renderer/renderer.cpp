#include "renderer/renderer.hpp"

#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

Renderer::Renderer(std::shared_ptr<MovingCamera> cam)
	: m_Cam(cam) {

}

size_t Renderer::addProgram(std::shared_ptr<Program> program) {
	size_t id = m_Programs.size();

	m_Programs.push_back(program);
	m_Objects.push_back(std::vector<RenderObject>());

	return id;
}

void Renderer::addObject(RenderObject&& object, size_t programId) {
	m_Objects[programId].push_back(object);
}

void Renderer::setDirLight(const DirLight& dirLight) {
	m_DirLight = dirLight;
}

void Renderer::setPointLight(const PointLight& pointLight, size_t idx) {
	m_PointLights[idx] = pointLight;
}

void Renderer::draw() {
	for (size_t i = 0; i < m_Objects.size(); i++) {
		std::shared_ptr<Program> program = m_Programs[i];

		program->set("uCamPos", m_Cam->m_Position);

		for (RenderObject& object : m_Objects[i]) {
			object.draw(*program);
		}
	}
}

void Renderer::updateProgramUniforms() {
	for (size_t i = 0; i < m_Programs.size(); i++) {
		updateProgramUniforms(i);
	}
}

void Renderer::updateProgramUniforms(size_t programId) {
	std::shared_ptr<Program> program = m_Programs[programId];

	// directional light
	program->set("uDirLight.direction", m_DirLight.direction);
	program->set("uDirLight.ambient", m_DirLight.ambient);
	program->set("uDirLight.diffuse", m_DirLight.diffuse);
	program->set("uDirLight.specular", m_DirLight.specular);

	// point lights
	for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
		PointLight pointLight = m_PointLights[i];

		program->set("uPointLights[" + std::to_string(i) + "].position", pointLight.position);
		program->set("uPointLights[" + std::to_string(i) + "].ambient", pointLight.ambient);
		program->set("uPointLights[" + std::to_string(i) + "].diffuse", pointLight.diffuse);
		program->set("uPointLights[" + std::to_string(i) + "].specular", pointLight.specular);
		program->set("uPointLights[" + std::to_string(i) + "].constant", pointLight.constant);
		program->set("uPointLights[" + std::to_string(i) + "].linear", pointLight.linear);
		program->set("uPointLights[" + std::to_string(i) + "].quadratic", pointLight.quadratic);
	}
}