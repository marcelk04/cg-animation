#include "movingcamera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>


#include <iostream>


inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

MovingCamera::MovingCamera(const glm::vec3& position, const glm::vec3& target) {
	m_Position = position;

	lookAt(target);

	update();
}

void MovingCamera::setResolution(const glm::vec2& resolution) {
	m_AspectRatio = resolution.x / resolution.y;

	m_UpToDate = false;
}

void MovingCamera::moveTo(const glm::vec3& position) {
	m_Position = position;

	m_UpToDate = false;
}

void MovingCamera::lookAt(const glm::vec3& target) {
	m_Direction = glm::normalize(target - m_Position);

	m_Pitch = glm::asin(-m_Direction.y);
	m_Yaw = glm::atan(m_Direction.z, m_Direction.x);

	m_UpToDate = false;
}

void MovingCamera::move(const glm::vec3& delta) {
	m_Position += delta;

	m_UpToDate = false;
}

void MovingCamera::rotate(const glm::vec2& delta) {
	m_Yaw -= delta.x;
	m_Pitch -= delta.y;

	if (glm::degrees(m_Pitch) > 89.0f) {
		m_Pitch = glm::radians(89.0f);
	}
	else if (glm::degrees(m_Pitch) < -89.0f) {
		m_Pitch = glm::radians(-89.0f);
	}

	m_Direction.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
	m_Direction.y = -glm::sin(m_Pitch);
	m_Direction.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

	m_UpToDate = false;
}

void MovingCamera::zoom(const float amount) {
	m_Fov = glm::clamp(m_Fov - amount, glm::radians(1.0f), glm::radians(90.0f));

	m_UpToDate = false;
}

bool MovingCamera::updateIfChanged() {
	if (!m_UpToDate) {
		update();

		return true;
	}

	return false;
}

void MovingCamera::update() {
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	m_Right = glm::normalize(glm::cross(up, -m_Direction));
	m_Up = glm::cross(-m_Direction, m_Right);

	m_View = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
	m_Projection = glm::perspective(m_Fov, m_AspectRatio, m_Near, m_Far);

	m_UpToDate = true;
}