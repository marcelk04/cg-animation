#pragma once

#include <glm/glm.hpp>

#include <cmath>

class Light {
public:
	glm::vec3 color;
};

class DirLight : public Light {
public:
	glm::vec3 direction;
};

class PointLight : public Light {
public:
	glm::vec3 position;
	float radius;

	// attenuation
	float constant;
	float linear;
	float quadratic;

	void calculateRadius() {
		float lightMax = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
		
		radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
	}
};