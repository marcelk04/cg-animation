#include "cinematic_engine/spline.hpp"

#include "framework/common.hpp"

glm::vec3 Spline::getPoint(float t) const {
	if (getNumCurves() == 1 || t < 1) {
		return Common::deCasteljau(m_Points[0], t);
	}

	uint32_t curveNum = static_cast<uint32_t>(t);

	if (curveNum >= getNumCurves()) {
		curveNum = getNumCurves() - 1;
	} else if (curveNum < 0) {
		curveNum = 0;
	}

	std::vector<glm::vec3> curve(1 + m_Points[curveNum].size());
	const std::vector<glm::vec3>& previousCurve = m_Points[curveNum - 1];

	curve[0] = previousCurve[previousCurve.size() - 1];

	for (size_t i = 0; i < m_Points[curveNum].size(); i++) {
		curve[i + 1] = m_Points[curveNum][i];
	}

	return Common::deCasteljau(curve, t - static_cast<float>(curveNum));
}

const std::vector<std::vector<glm::vec3>>& Spline::getPoints() const {
	return m_Points;
}

size_t Spline::getNumCurves() const {
	return m_Points.size();
}

void Spline::addCurve(const std::vector<glm::vec3>& curve) {
	m_Points.push_back(curve);
}
