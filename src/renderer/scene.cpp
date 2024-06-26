#include "renderer/scene.hpp"

#include <unordered_set>

void Scene::addRenderObject(RenderObject&& renderObject, size_t programId) {
	while (programId >= m_RenderObjects.size()) {
		m_RenderObjects.push_back(std::vector<RenderObject>());
	}

	m_RenderObjects[programId].push_back(renderObject);
}

void Scene::setDirLight(DirLight&& dirLight) {
	m_DirLight = std::make_optional<DirLight>(dirLight);
}

bool Scene::addPointLight(PointLight&& pointLight) {
	if (m_PointLights.size() >= MAX_NR_LIGHTS) {
		return false;
	}

	m_PointLights.push_back(pointLight);

	return true;
}

std::vector<std::vector<RenderObject>>& Scene::getRenderObjects() {
	return m_RenderObjects;
}

std::vector<RenderObject>& Scene::getRenderObjects(size_t programId) {
	return m_RenderObjects[programId];
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