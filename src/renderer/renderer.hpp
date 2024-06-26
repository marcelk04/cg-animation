#pragma once

#include "renderer/renderobject.hpp"
#include "renderer/light.hpp"
#include "renderer/scene.hpp"

#include "movingcamera.hpp"

#include "framework/mesh.hpp"
#include "framework/gl/program.hpp"
#include "framework/gl/texture.hpp"
#include "framework/gl/framebuffer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <array>
#include <memory>

class Renderer {
public:
	Renderer(std::shared_ptr<MovingCamera> cam, const glm::vec2& resolution);

	size_t addProgram(std::shared_ptr<Program> program);

	std::shared_ptr<Program> getProgram(size_t programId) { return m_Programs[programId]; }

	void draw(Scene& scene);

	void updateLightingUniforms(Scene& scene);

	void updateCamUniforms();
	void updateCamUniforms(size_t programId);

	void setResolution(const glm::vec2& resolution);

private:
	void generateTextures();
	void generateTexture(Texture& texture, GLint internalformat, GLenum format, GLenum type) const;

private:
	std::shared_ptr<MovingCamera> m_Cam;
	glm::vec2 m_Resolution;

	std::vector<std::shared_ptr<Program>> m_Programs;

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