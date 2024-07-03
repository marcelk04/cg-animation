#pragma once

#include <glm/glm.hpp>

#include <vector>

class Spline {
public:
	Spline() = default;

	glm::vec3 getPoint(float t) const;
	std::vector<std::vector<glm::vec3>>& getPoints();
	size_t getNumCurves() const;

	void addCurve(const std::vector<glm::vec3>& curve);

private:
	std::vector<std::vector<glm::vec3>> m_Points;
};