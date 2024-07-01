#include "renderer/light.hpp"


Light::Light()
	: m_Color(glm::vec3(1.0f)) {

}

glm::vec3 Light::getColor() const {
	return m_Color;
}

void Light::setColor(const glm::vec3& color) {
	m_Color = color;
}


DirLight::DirLight()
	: Light(), m_Direction(glm::vec3(1.0f)) {

}

glm::vec3 DirLight::getDirection() const {
	return m_Direction;
}

void DirLight::setDirection(const glm::vec3& direction) {
	m_Direction = glm::normalize(direction);
}


PointLight::PointLight()
	: Light(), m_Position(glm::vec3(0.0f)) {
	setAttenuationFactors(1.0f, 0.14f, 0.07f);
}

glm::vec3 PointLight::getPosition() const {
	return m_Position;
}

float PointLight::getRadius() const {
	return m_Radius;
}

float PointLight::getConstant() const {
	return m_Constant;
}

float PointLight::getLinear() const {
	return m_Linear;
}

float PointLight::getQuadratic() const {
	return m_Quadratic;
}

void PointLight::setPosition(const glm::vec3& position) {
	m_Position = position;
}

void PointLight::setAttenuationFactors(float constant, float linear, float quadratic) {
	m_Constant = constant;
	m_Linear = linear;
	m_Quadratic = quadratic;

	// update light radius
	float lightMax = std::fmaxf(std::fmaxf(m_Color.r, m_Color.g), m_Color.b);

	m_Radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
}
