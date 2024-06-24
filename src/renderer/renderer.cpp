#include "renderer/renderer.hpp"

#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

Renderer::Renderer(std::shared_ptr<MovingCamera> cam, const glm::vec2& resolution)
	: m_Cam(cam), m_Resolution(resolution) {
	generateTextures();

	m_LightingShader.load("deferred_lighting.vert", "deferred_lighting.frag");
	m_LightingShader.bindTextureUnit("uPosition", 0);
	m_LightingShader.bindTextureUnit("uNormal", 1);
	m_LightingShader.bindTextureUnit("uAlbedoSpec", 2);

	m_BlurShader.load("blurshader.vert", "blurshader.frag");
	m_BlurShader.bindTextureUnit("uColorBuffer", 1);

	m_HdrShader.load("hdrshader.vert", "hdrshader.frag");
	m_HdrShader.bindTextureUnit("uHdrBuffer", 0);
	m_HdrShader.bindTextureUnit("uBloomBuffer", 1);

	const std::vector<Mesh::VertexPCN> vertices = {
		{ glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f) },
		{ glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f) },
		{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f) },
		{ glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f) }
	};

	const std::vector<unsigned int> indices = { 0, 1, 2, 3, 2, 1 };

	m_Quad.load(vertices, indices);
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

void Renderer::setDirLight(DirLight&& dirLight) {
	m_DirLight = dirLight;
}

bool Renderer::addPointLight(PointLight&& pointLight) {
	if (m_PointLights.size() >= NR_POINT_LIGHTS) {
		return false;
	}

	m_PointLights.push_back(pointLight);

	return true;
}

void Renderer::draw() {
	// Geometry pass
	m_GBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < m_Objects.size(); i++) {
		std::shared_ptr<Program> program = m_Programs[i];

		for (RenderObject& object : m_Objects[i]) {
			object.draw(*program);
		}
	}

	// Lighting pass
	m_ColorBuffer.bind();
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	m_GPosition.bind(Texture::Type::TEX2D, 0);
	m_GNormal.bind(Texture::Type::TEX2D, 1);
	m_GAlbdedoSpec.bind(Texture::Type::TEX2D, 2);

	m_LightingShader.set("uCamPos", m_Cam->getPosition());
	m_LightingShader.bind();
	m_Quad.draw();

	// Blur pass
	bool horizontal = true, firstIteration = true;
	size_t amount = 10;

	m_BlurShader.bind();

	for (size_t i = 0; i < amount; i++) {
		int framebufferIdx = static_cast<int>(horizontal);

		m_BlurFramebuffers[framebufferIdx].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		
		m_BlurShader.set("uHorizontal", horizontal);

		if (firstIteration) {
			m_BrightColorTexture.bind(Texture::Type::TEX2D, 1);
		} else {
			m_BlurTextures[1 - framebufferIdx].bind(Texture::Type::TEX2D, 1);
		}

		m_Quad.draw();

		horizontal = !horizontal;
		firstIteration = false;
	}

	// HDR pass
	Framebuffer::bindDefault();
	glClear(GL_COLOR_BUFFER_BIT);

	m_ColorTexture.bind(Texture::Type::TEX2D, 0);
	m_BlurTextures[0].bind(Texture::Type::TEX2D, 1);
	m_HdrShader.bind();
	m_Quad.draw();
}

void Renderer::updateLightingUniforms() {
	// directional light
	m_LightingShader.set("uDirLight.direction", m_DirLight.direction);
	m_LightingShader.set("uDirLight.color", m_DirLight.color);

	// point lights
	for (size_t i = 0; i < m_PointLights.size(); i++) {
		PointLight pointLight = m_PointLights[i];

		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].position", pointLight.position);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].color", pointLight.color);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].radius", pointLight.radius);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].constant", pointLight.constant);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].linear", pointLight.linear);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].quadratic", pointLight.quadratic);
	}

	for (size_t i = m_PointLights.size(); i < NR_POINT_LIGHTS; i++) {
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].position", glm::vec3(0.0f));
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].color", glm::vec3(0.0f));
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].radius", 0.0f);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].constant", 1.0f);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].linear", 0.0f);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].quadratic", 0.0f);
	}
}

void Renderer::updateCamUniforms() {
	for (size_t i = 0; i < m_Programs.size(); i++) {
		updateCamUniforms(i);
	}
}

void Renderer::updateCamUniforms(size_t programId) {
	std::shared_ptr<Program> program = m_Programs[programId];

	program->set("uWorldToClip", m_Cam->projection() * m_Cam->view());
}

void Renderer::setResolution(const glm::vec2& resolution) {
	m_Resolution = resolution;
	generateTextures();
}

void Renderer::generateTextures() {
	generateTexture(m_GPosition, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR0, m_GPosition.handle);

	generateTexture(m_GNormal, GL_RGBA16_SNORM, GL_RGBA, GL_FLOAT);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR1, m_GNormal.handle);

	generateTexture(m_GAlbdedoSpec, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR2, m_GAlbdedoSpec.handle);

	generateTexture(m_GDepth, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
	m_GBuffer.attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::DEPTH, m_GDepth.handle);

	m_GBuffer.bind();
	std::array<GLenum, 3> drawGeomBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(drawGeomBuffers.size(), drawGeomBuffers.data());

	// fragment colors
	generateTexture(m_ColorTexture, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_ColorBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR0, m_ColorTexture.handle);

	// only bright fragment colors
	generateTexture(m_BrightColorTexture, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_ColorBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR1, m_BrightColorTexture.handle);

	m_ColorBuffer.bind();
	std::array<GLenum, 2> drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	// blur textures
	for (size_t i = 0; i < 2; i++) {
		m_BlurFramebuffers[i].bind();

		generateTexture(m_BlurTextures[i], GL_RGBA16F, GL_RGBA, GL_FLOAT);

		m_BlurFramebuffers[i].attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR0, m_BlurTextures[i].handle);
	}
}

void Renderer::generateTexture(Texture& texture, GLint internalformat, GLenum format, GLenum type) const {
	texture.bind(Texture::Type::TEX2D);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Resolution.x, m_Resolution.y, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}