#pragma once

#include <glm/glm.hpp>

#include <cmath>

class Light {
public:
	Light();

	glm::vec3 getColor() const;

	void setColor(const glm::vec3& color);

protected:
	glm::vec3 m_Color;
};

class DirLight : public Light {
public:
	DirLight();

	glm::vec3 getDirection() const;

	void setDirection(const glm::vec3& direction);

protected:
	glm::vec3 m_Direction; // normalized direction toward light source
};

class PointLight : public Light {
public:
	PointLight();

	glm::vec3 getPosition() const;
	float getRadius() const;
	float getConstant() const;
	float getLinear() const;
	float getQuadratic() const;

	void setPosition(const glm::vec3& position);
	void setAttenuationFactors(float constant, float linear, float quadratic);

protected:
	glm::vec3 m_Position;
	float m_Radius;

	// attenuation
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
};