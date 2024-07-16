#include "renderer/renderer.hpp"

#include "resourcemanager.hpp"
#include "framework/common.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Renderer::Renderer(std::shared_ptr<MovingCamera> cam, const glm::vec2& resolution)
	: m_Cam(cam), m_Resolution(resolution), m_Scene(nullptr), m_ShowCameraControlPoints(false) {
	generateTextures();

	m_SimpleGeometryShader.load("simple_geometry.vert", "simple_geometry.frag");

	m_DepthShader.load("depthshader.vert", "depthshader.frag");

	m_CubeDepthShader.load("cubedepthshader.vert", "cubedepthshader.geom", "cubedepthshader.frag");

	m_LightingShader.load("deferred_lighting.vert", "deferred_lighting.frag");
	m_LightingShader.bindTextureUnit("uPosition", 0);
	m_LightingShader.bindTextureUnit("uNormal", 1);
	m_LightingShader.bindTextureUnit("uAlbedoSpec", 2);
	m_LightingShader.bindTextureUnit("uDShadowMap", 3);
	m_LightingShader.bindTextureUnit("uOShadowMap", 4);

	m_BlurShader.load("blurshader.vert", "blurshader.frag");
	m_BlurShader.bindTextureUnit("uColorBuffer", 1);

	m_HdrShader.load("hdrshader.vert", "hdrshader.frag");
	m_HdrShader.bindTextureUnit("uHdrBuffer", 0);
	m_HdrShader.bindTextureUnit("uBloomBuffer", 1);

	// screen size quad
	const std::vector<Mesh::VertexPCN> vertices = {
		{ glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f) },
		{ glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f) },
		{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f) },
		{ glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f) }
	};

	const std::vector<unsigned int> indices = { 0, 1, 2, 3, 2, 1 };

	m_Quad.load(vertices, indices);

	ResourceManager::loadMesh("meshes/highpolysphere.obj", "sphere");
}

void Renderer::update(float dt) {
	if (m_Scene != nullptr) {
		m_Scene->update(dt);
	}
}

void Renderer::draw() {
	// only calculate shadow map if scene has a directional light
	if (m_Scene->getDirLight().has_value()) {
		directionalShadowPass(*m_Scene);
	}

	if (m_Scene->getRenderObjects().size() > 0) {
		omnidirectionalShadowPass(*m_Scene);
	}

	geometryPass(*m_Scene);

	lightingPass(m_Scene->getDirLight().has_value());

	int blurBuffer = blurPass(m_BlurAmount);

	hdrPass(blurBuffer, m_Exposure, m_Gamma);
}

size_t Renderer::addProgram(std::shared_ptr<Program> program) {
	size_t id = m_Programs.size();

	m_Programs.push_back(program);

	return id;
}

void Renderer::setScene(std::shared_ptr<Scene> scene) {
	m_Scene = scene;

	updateLightingUniforms();
}

void Renderer::setResolution(const glm::vec2& resolution) {
	m_Resolution = resolution;
	generateTextures();
}

void Renderer::showCameraControlPoints(bool showPoints) {
	m_ShowCameraControlPoints = showPoints;

	if (showPoints) {
		regenerateCameraControlRenderObjects();
	}
}

void Renderer::updateLightingUniforms() {
	// directional light
	if (m_Scene->getDirLight().has_value()) {
		DirLight& dirLight = m_Scene->getDirLight().value();

		m_LightingShader.set("uDirLight.direction", dirLight.getDirection());
		m_LightingShader.set("uDirLight.color", dirLight.getColor());

		float nearPlane = 1.0f, farPlane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(5.0f * dirLight.getDirection(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		m_DepthShader.set("uLightSpaceMatrix", lightSpaceMatrix);
		m_LightingShader.set("uLightSpaceMatrix", lightSpaceMatrix);
	} else {
		m_LightingShader.set("uDirLight.direction", glm::vec3(1.0f));
		m_LightingShader.set("uDirLight.color", glm::vec3(0.0f));
	}

	if (m_Scene->getPointLights().size() > 0) {
		PointLight& light = m_Scene->getPointLight(0);

		float aspectRatio = static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT);
		float near = 0.75f;
		float far = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspectRatio, near, far);

		std::vector<glm::mat4> shadowTransforms(6);
		shadowTransforms[0] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // front
		shadowTransforms[1] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // back
		shadowTransforms[2] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // up
		shadowTransforms[3] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // down
		shadowTransforms[4] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // right
		shadowTransforms[5] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // left

		m_CubeDepthShader.set("uFar", far);
		m_CubeDepthShader.set("uLightPosition", light.getPosition());
		m_LightingShader.set("uFar", far);
		m_LightingShader.set("uShadowLightPos", light.getPosition());

		for (uint32_t i = 0; i < 6; i++) {
			m_CubeDepthShader.set("uShadowTransforms[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
	}

	// point lights
	for (size_t i = 0; i < m_Scene->getPointLights().size(); i++) {
		PointLight& pointLight = m_Scene->getPointLight(i);

		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].position", pointLight.getPosition());
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].color", pointLight.getColor());
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].radius", pointLight.getRadius());
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].constant", pointLight.getConstant());
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].linear", pointLight.getLinear());
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].quadratic", pointLight.getQuadratic());
	}

	// add dummy lights
	for (size_t i = m_Scene->getPointLights().size(); i < m_Scene->MAX_NR_LIGHTS; i++) {
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].position", glm::vec3(0.0f));
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].color", glm::vec3(0.0f));
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].radius", 0.0f);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].constant", 1.0f);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].linear", 0.0f);
		m_LightingShader.set("uPointLights[" + std::to_string(i) + "].quadratic", 0.0f);
	}
}

void Renderer::updateCamUniforms() {
	m_SimpleGeometryShader.set("uWorldToClip", m_Cam->projection() * m_Cam->view());

	for (size_t i = 0; i < m_Programs.size(); i++) {
		updateCamUniforms(i);
	}
}

void Renderer::updateCamUniforms(size_t programId) {
	std::shared_ptr<Program> program = m_Programs[programId];

	program->set("uWorldToClip", m_Cam->projection() * m_Cam->view());
}

void Renderer::directionalShadowPass(Scene& scene) {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_DShadowBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	drawScene(scene);

	glViewport(0, 0, m_Resolution.x, m_Resolution.y);
	glCullFace(GL_BACK);
}

void Renderer::omnidirectionalShadowPass(Scene& scene) {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_OShadowBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	drawScene(scene);

	glViewport(0, 0, m_Resolution.x, m_Resolution.y);
	glCullFace(GL_BACK);
}

void Renderer::geometryPass(Scene& scene) {
	m_GBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene(scene);

	if (scene.getParticleSystem().has_value()) {
		scene.getParticleSystem()->render(m_Cam->projection() * m_Cam->view());
	}

	// draw camera control points
	if (m_ShowCameraControlPoints && m_Scene->getCameraController().has_value()) {
		CameraController& camController = m_Scene->getCameraController().value();

		for (RenderObject& controlPoint : m_CameraControlRenderObjects) {
			controlPoint.draw(m_SimpleGeometryShader);
		}
	}
}

void Renderer::lightingPass(bool enableShadows) {
	m_ColorBuffer.bind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT);

	m_GPosition.bind(Texture::Type::TEX2D, 0);
	m_GNormal.bind(Texture::Type::TEX2D, 1);
	m_GAlbdedoSpec.bind(Texture::Type::TEX2D, 2);
	m_DShadowMap.bind(Texture::Type::TEX2D, 3);
	m_OShadowCubeMap.bind(Texture::Type::CUBE_MAP, 4);

	m_LightingShader.set("uCamPos", m_Cam->getPosition());
	m_LightingShader.set("uEnableShadows", enableShadows);
	m_LightingShader.bind();

	m_Quad.draw();
}

int Renderer::blurPass(int amount) {
	bool horizontal = true, firstIteration = true;

	m_BlurShader.bind();
	glDisable(GL_DEPTH_TEST);

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

	int last = static_cast<int>(!horizontal);

	return last;
}

void Renderer::hdrPass(int blurBuffer, float exposure, float gamma) {
	Framebuffer::bindDefault();
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	m_ColorTexture.bind(Texture::Type::TEX2D, 0);
	m_BlurTextures[blurBuffer].bind(Texture::Type::TEX2D, 1);

	m_HdrShader.set("uExposure", exposure);
	m_HdrShader.set("uGamma", gamma);
	m_HdrShader.bind();

	m_Quad.draw();
}

void Renderer::drawScene(Scene& scene) {
	for (size_t i = 0; i < m_Programs.size(); i++) {
		std::shared_ptr<Program> program = m_Programs[i];

		// draw all render objects that use this shader
		for (RenderObject& object : scene.getRenderObjects(i)) {
			object.draw(*program);
		}
	}
}

void Renderer::generateTextures() {
	// directional shadows
	m_DShadowBuffer.bind();

	generateTexture(m_DShadowMap, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, glm::vec2(SHADOW_WIDTH, SHADOW_HEIGHT));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DShadowMap.handle, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// omnidirectional shadows
	m_OShadowBuffer.bind();

	m_OShadowCubeMap.bind(Texture::Type::CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (uint32_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_OShadowCubeMap.handle, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// geometry buffer
	m_GBuffer.bind();

	generateTexture(m_GPosition, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR0, m_GPosition.handle);

	generateTexture(m_GNormal, GL_RGBA16_SNORM, GL_RGBA, GL_FLOAT);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR1, m_GNormal.handle);

	generateTexture(m_GAlbdedoSpec, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR2, m_GAlbdedoSpec.handle);

	generateTexture(m_GDepth, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
	m_GBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::DEPTH, m_GDepth.handle);

	std::array<GLenum, 3> drawGeomBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(drawGeomBuffers.size(), drawGeomBuffers.data());

	// fragment colors
	m_ColorBuffer.bind();

	generateTexture(m_ColorTexture, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_ColorBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR0, m_ColorTexture.handle);

	// only bright fragment colors
	generateTexture(m_BrightColorTexture, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_ColorBuffer.attach(Framebuffer::Type::READ_AND_DRAW, Framebuffer::Attachment::COLOR1, m_BrightColorTexture.handle);

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
	generateTexture(texture, internalformat, format, type, m_Resolution);
}

void Renderer::generateTexture(Texture& texture, GLint internalformat, GLenum format, GLenum type, const glm::vec2& resolution) const {
	texture.bind(Texture::Type::TEX2D);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, resolution.x, resolution.y, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::regenerateCameraControlRenderObjects() {
	if (!m_Scene->getCameraController().has_value()) {
		return;
	}

	CameraController& camController = m_Scene->getCameraController().value();

	Material movementPointMaterial{
		glm::vec3(0.1f, 0.9f, 0.2f),
		0.0f
	};

	Material targetPointMaterial{
		glm::vec3(1.0f, 0.1f, 0.2f),
		0.0f
	};

	// remove old points
	m_CameraControlRenderObjects.clear();

	// movement control points
	for (const auto& curve : camController.getMovementControlPoints()) {
		for (const auto& point : curve) {
			RenderObject obj;
			obj.setMesh("sphere");
			obj.setPosition(point);
			obj.setScale(0.1f);
			obj.setMaterial(movementPointMaterial);
			m_CameraControlRenderObjects.push_back(std::move(obj));
		}
	}

	// target control points
	for (const auto& curve : camController.getTargetControlPoints()) {
		for (const auto& point : curve) {
			RenderObject obj;
			obj.setMesh("sphere");
			obj.setPosition(point);
			obj.setScale(0.1f);
			obj.setMaterial(targetPointMaterial);
			m_CameraControlRenderObjects.push_back(std::move(obj));
		}
	}
}