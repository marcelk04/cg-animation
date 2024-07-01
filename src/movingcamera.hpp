#pragma once

#include <glm/glm.hpp>

class MovingCamera {
public:
	MovingCamera(const glm::vec3& position, const glm::vec3& target);

	const glm::mat4& view() const { return m_View; }
	const glm::mat4& projection() const { return m_Projection; }

	const glm::vec3& getPosition() const { return m_Position; }
	const glm::vec3& getDirection() const { return m_Direction; }
	const glm::vec3& getUp() const { return m_Up; }
	const glm::vec3& getRight() const { return m_Right; }

	float getFov() const { return m_Fov; }
	float getNear() const { return m_Near; }
	float getFar() const { return m_Far; }
	float getAspectRatio() const { return m_AspectRatio; }

	float getYaw() const { return m_Yaw; }
	float getPitch() const { return m_Pitch; }

	void setResolution(const glm::vec2& resolution);

	void moveTo(const glm::vec3& position);
	void lookAt(const glm::vec3& target);

	void move(const glm::vec3& delta);
	void rotate(const glm::vec2& delta);
	void zoom(const float amount);

	bool updateIfChanged();

private:
	void update();

private:
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
	float m_AspectRatio = 800.0f / 600.0f;

	float m_Yaw;
	float m_Pitch;

	bool m_UpToDate;
};