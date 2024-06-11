#pragma once

#include <glm/glm.hpp>

class MovingCamera {
public:
	MovingCamera(const glm::vec3& position, const glm::vec3& target);

	glm::mat4& view() { return m_View; }
	glm::mat4& projection() { return m_Projection; }

	void moveTo(const glm::vec3& position);
	void lookAt(const glm::vec3& target);

	void move(const glm::vec3& delta);
	void rotate(const glm::vec2& delta);
	void zoom(const float amount);

	bool updateIfChanged();

private:
	void update();

public:
	// position
	glm::vec3 m_Position;

	// orientation
	glm::vec3 m_Direction;
	glm::vec3 m_Up;
	glm::vec3 m_Right;

	// matrices
	glm::mat4 m_View;
	glm::mat4 m_Projection;


	float m_Fov = glm::radians(60.0f);
	float m_Near = 0.1f;
	float m_Far = 100.0f;
	float m_AspectRatio = 1.0f;

	float m_Yaw;
	float m_Pitch;

	bool m_UpToDate;
};