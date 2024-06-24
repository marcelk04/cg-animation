#pragma once

#include "renderer/renderobject.hpp"
#include "renderer/light.hpp"

#include "movingcamera.hpp"

#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"
#include "framework/gl/framebuffer.hpp"

#include <glm/glm.hpp>

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
	bool addPointLight(PointLight&& pointLight);

	std::shared_ptr<Program> getProgram(size_t programId) { return m_Programs[programId]; }
	std::vector<RenderObject>& getRenderObjects(size_t programId) { return m_Objects[programId]; }
	DirLight& getDirLight() { return m_DirLight; }
	std::vector<PointLight>& getPointLights() { return m_PointLights; }

	void draw();

	void updateLightingUniforms();

	void updateCamUniforms();
	void updateCamUniforms(size_t programId);

	void setResolution(const glm::vec2& resolution);

private:
	void generateTextures();
	void generateTexture(Texture& texture, GLint internalformat, GLenum format, GLenum type) const;

public:
	std::shared_ptr<MovingCamera> m_Cam;
	glm::vec2 m_Resolution;

	std::vector<std::shared_ptr<Program>> m_Programs;

	// scene info
	std::vector<std::vector<RenderObject>> m_Objects;

	DirLight m_DirLight;
	std::vector<PointLight> m_PointLights;

	Mesh m_Quad;

	// deferred shading
	Texture m_GPosition;
	Texture m_GNormal;
	Texture m_GAlbdedoSpec;
	Texture m_GDepth;
	Framebuffer m_GBuffer;

	Program m_LightingShader;

	// hdr effects
	Texture m_ColorTexture;
	Texture m_BrightColorTexture; // required for bloom
	Framebuffer m_ColorBuffer;

	Program m_HdrShader;

	std::array<Framebuffer, 2> m_BlurFramebuffers;
	std::array<Texture, 2> m_BlurTextures;

	Program m_BlurShader;
};