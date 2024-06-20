#pragma once

#include <glm/glm.hpp>

class Light {
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class DirLight : public Light {
public:
	glm::vec3 direction;
};

class PointLight : public Light {
public:
	glm::vec3 position;

	// attenuation
	float constant;
	float linear;
	float quadratic;
};