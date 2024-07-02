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
	const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

public:
	Renderer(std::shared_ptr<MovingCamera> cam, const glm::vec2& resolution);

	size_t addProgram(std::shared_ptr<Program> program);

	std::shared_ptr<Program> getProgram(size_t programId) { return m_Programs[programId]; }
	float getExposure() const { return m_Exposure; }
	float getGamma() const { return m_Gamma; }
	int getBlurAmount() const { return m_BlurAmount; }

	void setExposure(float exposure) { m_Exposure = exposure; }
	void setGamma(float gamma) { m_Gamma = gamma; }
	void setBlurAmount(int blurAmount) { m_BlurAmount = blurAmount; }
	void setResolution(const glm::vec2& resolution);

	void draw(Scene& scene);

	void updateLightingUniforms(Scene& scene);

	void updateCamUniforms();
	void updateCamUniforms(size_t programId);

private:
	void shadowPass(Scene& scene);
	void geometryPass(Scene& scene);
	void lightingPass(bool enableShadows);
	int blurPass(int amount);
	void hdrPass(int blurBuffer, float exposure, float gamma);

	void generateTextures();
	void generateTexture(Texture& texture, GLint internalformat, GLenum format, GLenum type) const;
	void generateTexture(Texture& texture, GLint internalformat, GLenum format, GLenum type, const glm::vec2& resolution) const;

private:
	std::shared_ptr<MovingCamera> m_Cam;
	glm::vec2 m_Resolution;

	std::vector<std::shared_ptr<Program>> m_Programs;

	Mesh m_Quad;

	// shadow mapping
	Texture m_ShadowMap;
	Framebuffer m_ShadowBuffer;

	glm::mat4 m_LightSpaceMatrix;

	Program m_DepthShader;

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

	float m_Exposure = 1.0f;
	float m_Gamma = 2.2f;
	int m_BlurAmount = 10;
};