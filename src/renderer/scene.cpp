#include "renderer/scene.hpp"

#include <GLFW/glfw3.h>

Scene::Scene()
	: m_RenderObjects(), m_DirLight(std::nullopt), m_PointLights(), m_CameraController(std::nullopt) {
}

void Scene::update(float dt) {
	if (m_CameraController.has_value()) {
		m_CameraController->update(dt);
	}

	if (m_ParticleSystem.has_value()) {
		m_ParticleSystem->update(static_cast<float>(glfwGetTime()));
	}
}

size_t Scene::addRenderObject(RenderObject&& renderObject, size_t programId) {
	while (programId >= m_RenderObjects.size()) {
		m_RenderObjects.push_back(std::vector<RenderObject>());
	}

	size_t id = m_RenderObjects[programId].size();

	m_RenderObjects[programId].push_back(renderObject);

	return id;
}

void Scene::setDirLight(DirLight&& dirLight) {
	m_DirLight = std::make_optional<DirLight>(std::move(dirLight));
}

bool Scene::addPointLight(PointLight&& pointLight) {
	if (m_PointLights.size() >= MAX_NR_LIGHTS) {
		return false;
	}

	m_PointLights.push_back(pointLight);

	return true;
}

void Scene::setCameraController(CameraController&& cameraController) {
	m_CameraController = std::make_optional<CameraController>(cameraController);
}

void Scene::setParticleSystem(ParticleSystem&& particleSystem) {
	m_ParticleSystem = std::make_optional<ParticleSystem>(std::move(particleSystem));
}

bool Scene::removeRenderObject(size_t programId, size_t objectId) {
	if (programId >= m_RenderObjects.size()) {
		return false;
	}

	if (objectId >= m_RenderObjects[programId].size()) {
		return false;
	}

	m_RenderObjects[programId].erase(m_RenderObjects[programId].begin() + objectId);

	return true;
}

std::vector<std::vector<RenderObject>>& Scene::getRenderObjects() {
	return m_RenderObjects;
}

std::vector<RenderObject>& Scene::getRenderObjects(size_t programId) {
	while (programId >= m_RenderObjects.size()) {
		m_RenderObjects.push_back(std::vector<RenderObject>());
	}

	return m_RenderObjects[programId];
}

RenderObject& Scene::getRenderObject(size_t programId, size_t objectId) {
	return getRenderObjects(programId)[objectId];
}

std::optional<DirLight>& Scene::getDirLight() {
	return m_DirLight;
}

std::vector<PointLight>& Scene::getPointLights() {
	return m_PointLights;
}

PointLight& Scene::getPointLight(size_t i) {
	return m_PointLights[i];
}

std::optional<CameraController>& Scene::getCameraController() {
	return m_CameraController;
}

std::optional<ParticleSystem>& Scene::getParticleSystem() {
	return m_ParticleSystem;
}