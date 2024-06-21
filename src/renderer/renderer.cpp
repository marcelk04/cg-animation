#include "renderer/renderer.hpp"

#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

Renderer::Renderer(std::shared_ptr<MovingCamera> cam, const glm::vec2& resolution)
	: m_Cam(cam), m_Resolution(resolution) { // pls dont change the window size :(
	m_ColorTexture.bind(Texture::Type::TEX2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Resolution.x, m_Resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_HdrBuffer.attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::COLOR0, m_ColorTexture.handle);

	m_BrightColorTexture.bind(Texture::Type::TEX2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Resolution.x, m_Resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_HdrBuffer.attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::COLOR1, m_BrightColorTexture.handle);

	m_DepthTexture.bind(Texture::Type::TEX2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, m_Resolution.x, m_Resolution.y, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_HdrBuffer.attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::DEPTH, m_DepthTexture.handle);

	std::array<GLenum, 2> drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	m_HdrBuffer.bind();
	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	m_HdrShader.load("hdrshader.vert", "hdrshader.frag");
	m_HdrShader.bindTextureUnit("uHdrBuffer", 0);
	m_HdrShader.bindTextureUnit("uBloomBuffer", 1);

	m_BlurShader.load("blurshader.vert", "blurshader.frag");
	m_BlurShader.bindTextureUnit("uColorBuffer", 1);

	// generate blur framebuffers and textures
	for (size_t i = 0; i < 2; i++) {
		m_BlurFramebuffers[i].bind();
		m_BlurTextures[i].bind(Texture::Type::TEX2D);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Resolution.x, m_Resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_BlurFramebuffers[i].attach(Framebuffer::Type::DRAW, Framebuffer::Attachment::COLOR0, m_BlurTextures[i].handle);
	}

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

void Renderer::setPointLight(PointLight&& pointLight, size_t idx) {
	m_PointLights[idx] = pointLight;
}

void Renderer::draw() {
	// Geometry pass
	m_HdrBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < m_Objects.size(); i++) {
		std::shared_ptr<Program> program = m_Programs[i];

		program->set("uCamPos", m_Cam->getPosition());

		for (RenderObject& object : m_Objects[i]) {
			object.draw(*program);
		}
	}

	// Blur pass
	bool horizontal = true, firstIteration = true;
	size_t amount = 10;

	m_BlurShader.bind();

	for (size_t i = 0; i < amount; i++) {
		int framebufferIdx = static_cast<int>(horizontal);

		m_BlurFramebuffers[framebufferIdx].bind();
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
	glDisable(GL_DEPTH_TEST);

	m_ColorTexture.bind(Texture::Type::TEX2D, 0);
	m_BlurTextures[0].bind(Texture::Type::TEX2D, 1);
	m_HdrShader.bind();
	m_Quad.draw();
}

void Renderer::updateLightingUniforms() {
	for (size_t i = 0; i < m_Programs.size(); i++) {
		updateLightingUniforms(i);
	}
}

void Renderer::updateLightingUniforms(size_t programId) {
	std::shared_ptr<Program> program = m_Programs[programId];

	// directional light
	program->set("uDirLight.direction", m_DirLight.direction);
	program->set("uDirLight.ambient", m_DirLight.ambient);
	program->set("uDirLight.diffuse", m_DirLight.diffuse);
	program->set("uDirLight.specular", m_DirLight.specular);

	// point lights
	for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
		PointLight pointLight = m_PointLights[i];

		program->set("uPointLights[" + std::to_string(i) + "].position", pointLight.position);
		program->set("uPointLights[" + std::to_string(i) + "].ambient", pointLight.ambient);
		program->set("uPointLights[" + std::to_string(i) + "].diffuse", pointLight.diffuse);
		program->set("uPointLights[" + std::to_string(i) + "].specular", pointLight.specular);
		program->set("uPointLights[" + std::to_string(i) + "].constant", pointLight.constant);
		program->set("uPointLights[" + std::to_string(i) + "].linear", pointLight.linear);
		program->set("uPointLights[" + std::to_string(i) + "].quadratic", pointLight.quadratic);
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