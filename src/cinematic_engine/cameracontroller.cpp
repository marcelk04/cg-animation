#include "cinematic_engine/cameracontroller.hpp"

#include <iostream>

CameraController::CameraController()
	: m_Enabled(true), m_T(0.0f), m_Duration(1.0f) {

}

CameraController::CameraController(std::shared_ptr<MovingCamera> camera, Spline&& movementSpline, Spline&& targetSpline, float duration)
	: m_Cam(camera), m_MovementSpline(movementSpline), m_TargetSpline(targetSpline), m_Enabled(true), m_T(0.0f), m_Duration(duration) {

}

void CameraController::update(float dt) {
	if (m_Enabled) {
		m_T += dt;

		float progress = 1.0f - (m_Duration - m_T) / m_Duration;

		if (progress >= 1.0f) return;

		float movementT = progress * static_cast<float>(m_MovementSpline.getNumCurves());
		float targetT = progress * static_cast<float>(m_TargetSpline.getNumCurves());

		m_Cam->moveTo(m_MovementSpline.getPoint(movementT));
		m_Cam->lookAt(m_TargetSpline.getPoint(targetT));
	}
}

void CameraController::setCamera(std::shared_ptr<MovingCamera> camera) {
	m_Cam = camera;
}

void CameraController::setMovementSpline(Spline&& spline) {
	m_MovementSpline = spline;
}

void CameraController::setTargetSpline(Spline&& spline) {
	m_TargetSpline = spline;
}

void CameraController::setEnabled(bool enabled) {
	m_Enabled = enabled;
}

void CameraController::setDuration(float duration) {
	m_Duration = duration;
}

const std::vector<std::vector<glm::vec3>>& CameraController::getMovementControlPoints() const {
	return m_MovementSpline.getPoints();
}

const std::vector<std::vector<glm::vec3>>& CameraController::getTargetControlPoints() const {
	return m_TargetSpline.getPoints();
}

bool CameraController::isEnabled() const {
	return m_Enabled;
}