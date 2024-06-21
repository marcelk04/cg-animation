#pragma once

#include "renderer/renderobject.hpp"
#include "renderer/light.hpp"

#include "movingcamera.hpp"

#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"
#include "framework/gl/framebuffer.hpp"

#include <vector>
#include <array>
#include <memory>

#define NR_POINT_LIGHTS 4

class Renderer {
public:
	Renderer(std::shared_ptr<MovingCamera> cam, const glm::vec2& resolution);

	size_t addProgram(std::shared_ptr<Program> program);
	void addObject(RenderObject&& object, size_t programId);
	void setDirLight(DirLight&& dirLight);
	void setPointLight(PointLight&& pointLight, size_t idx);

	std::shared_ptr<Program> getProgram(size_t programId) { return m_Programs[programId]; }
	std::vector<RenderObject>& getRenderObjects(size_t programId) { return m_Objects[programId]; }
	DirLight& getDirLight() { return m_DirLight; }
	std::array<PointLight, NR_POINT_LIGHTS>& getPointLights() { return m_PointLights; }

	void draw();

	void updateLightingUniforms();
	void updateLightingUniforms(size_t programId);

	void updateCamUniforms();
	void updateCamUniforms(size_t programId);

public:
	std::shared_ptr<MovingCamera> m_Cam;
	glm::vec2 m_Resolution;

	std::vector<std::shared_ptr<Program>> m_Programs;

	// scene info
	std::vector<std::vector<RenderObject>> m_Objects;

	DirLight m_DirLight;
	std::array<PointLight, NR_POINT_LIGHTS> m_PointLights;

	// hdr effects
	Texture m_ColorTexture;
	Texture m_BrightColorTexture; // required for bloom
	Texture m_DepthTexture;
	Framebuffer m_HdrBuffer;

	Program m_HdrShader;

	std::array<Framebuffer, 2> m_BlurFramebuffers;
	std::array<Texture, 2> m_BlurTextures;

	Program m_BlurShader;

	Mesh m_Quad;
};