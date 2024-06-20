#pragma once

#include "renderer/renderobject.hpp"
#include "renderer/light.hpp"

#include "movingcamera.hpp"

#include "framework/gl/program.hpp"

#include <vector>
#include <array>
#include <memory>

#define NR_POINT_LIGHTS 4

class Renderer {
public:
	Renderer(std::shared_ptr<MovingCamera> cam);

	size_t addProgram(std::shared_ptr<Program> program);
	void addObject(RenderObject&& object, size_t programId);
	void setDirLight(const DirLight& dirLight);
	void setPointLight(const PointLight& pointLight, size_t idx);

	void draw();

	void updateProgramUniforms();
	void updateProgramUniforms(size_t programId);

public:

	std::shared_ptr<MovingCamera> m_Cam;

	std::vector<std::shared_ptr<Program>> m_Programs;

	std::vector<std::vector<RenderObject>> m_Objects;

	DirLight m_DirLight;
	std::array<PointLight, NR_POINT_LIGHTS> m_PointLights;
};