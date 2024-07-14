#pragma once

#include "cinematic_engine/movingcamera.hpp"
#include "cinematic_engine/spline.hpp"

#include <glm/glm.hpp>

#include <memory>

class CameraController {
public:
	CameraController();
	CameraController(std::shared_ptr<MovingCamera> camera, Spline&& movementSpline, Spline&& targetSpline, float duration);

	void update(float dt);

	void setCamera(std::shared_ptr<MovingCamera> camera);
	void setMovementSpline(Spline&& spline);
	void setTargetSpline(Spline&& spline);
	void setEnabled(bool enabled);
	void setDuration(float duration);

	const std::vector<std::vector<glm::vec3>>& getMovementControlPoints() const;
	const std::vector<std::vector<glm::vec3>>& getTargetControlPoints() const;
	bool isEnabled() const;

private:
	std::shared_ptr<MovingCamera> m_Cam;
	Spline m_MovementSpline;
	Spline m_TargetSpline;

	bool m_Enabled;
	float m_T;
	float m_Duration;
};