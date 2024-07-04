#pragma once

#include "renderer/light.hpp"
#include "renderer/renderobject.hpp"

#include "cinematic_engine/cameracontroller.hpp"

#include "framework/gl/program.hpp"

#include <vector>
#include <memory>
#include <optional>

class Scene {
public:
	const size_t MAX_NR_LIGHTS = 5;

public:
	Scene() = default;

	void update(float dt);

	size_t addRenderObject(RenderObject&& renderObject, size_t programId);
	void setDirLight(DirLight&& dirLight);
	bool addPointLight(PointLight&& pointLight);
	void setCameraController(CameraController&& cameraController);

	std::vector<std::vector<RenderObject>>& getRenderObjects();
	std::vector<RenderObject>& getRenderObjects(size_t programId);
	RenderObject& getRenderObject(size_t programId, size_t objectId);
	std::optional<DirLight>& getDirLight();
	std::vector<PointLight>& getPointLights();
	PointLight& getPointLight(size_t i);
	std::optional<CameraController>& getCameraController();

private:
	std::vector<std::vector<RenderObject>> m_RenderObjects;

	std::optional<DirLight> m_DirLight;
	std::vector<PointLight> m_PointLights;

	std::optional<CameraController> m_CameraController;
};