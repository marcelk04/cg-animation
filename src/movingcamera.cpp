#include "movingcamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

MovingCamera::MovingCamera(const glm::vec3& position, const glm::vec3& target) {
	m_Position = position;

	update();
}

void MovingCamera::move(const glm::vec3& delta) {
	m_Position += delta;

	m_UpToDate = false;
}

void MovingCamera::rotate(const glm::vec2& delta) {
	m_Yaw -= delta.x;
	m_Pitch += delta.y;

	if (m_Pitch > 89.0f) {
		m_Pitch = 89.0f;
	}
	else if (m_Pitch < -89.0f) {
		m_Pitch = -89.0f;
	}

	m_UpToDate = false;
}

void MovingCamera::zoom(const float amount) {
	m_Fov = glm::clamp(m_Fov - amount, 1.0f, 90.0f);

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
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_Direction.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	m_Direction.y = glm::sin(glm::radians(m_Pitch));
	m_Direction.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

	m_Direction = glm::normalize(m_Direction);
	m_Right = glm::normalize(glm::cross(up, -m_Direction));
	m_Up = glm::cross(-m_Direction, m_Right);

	m_View = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
	m_Projection = glm::perspective(m_Fov, m_AspectRatio, m_Near, m_Far);

	m_UpToDate = true;
}