#pragma once

#include "renderer/light.hpp"
#include "renderer/renderobject.hpp"

#include "framework/gl/program.hpp"

#include <vector>
#include <memory>
#include <optional>

class Scene {
public:
	const size_t MAX_NR_LIGHTS = 4;
	
public:
	Scene() = default;

	void addRenderObject(RenderObject&& renderObject, size_t programId);
	void setDirLight(DirLight&& dirLight);
	bool addPointLight(PointLight&& pointLight);

	std::vector<std::vector<RenderObject>>& getRenderObjects();
	std::vector<RenderObject>& getRenderObjects(size_t programId);
	std::optional<DirLight>& getDirLight();
	std::vector<PointLight>& getPointLights();
	PointLight& getPointLight(size_t i);

private:
	std::vector<std::vector<RenderObject>> m_RenderObjects;

	std::optional<DirLight> m_DirLight;
	std::vector<PointLight> m_PointLights;
};