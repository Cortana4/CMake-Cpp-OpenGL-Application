#pragma once
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "window.h"


class Camera
{
public:
	enum Direction
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	Camera(
		const glm::vec3& position,
		const glm::vec3& target = glm::vec3(0.0f, 0.0f, 0.0f),
		GLfloat nearRenderLimit = 0.1f,
		GLfloat farRenderLimit = 100.0f,
		GLfloat fov = 45.0f
	);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjMatrix(GLfloat width, GLfloat height) const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getXAxis() const;
	const glm::vec3& getYAxis() const;
	const glm::vec3& getZAxis() const;

	GLfloat getNearRenderLimit() const;
	GLfloat getFarRenderLimit() const;
	GLfloat getFov() const;

	std::function<void(Window*)> getKeyCallback();
	std::function<void(Window*)> getCursorPosCallback();
	std::function<void(Window*)> getScrollCallback();

	void setPosition(const glm::vec3& position);
	void setTarget(const glm::vec3& target);
	void setNearRenderLimit(GLfloat limit);
	void setFarRenderLimit(GLfloat limit);
	void setFov(GLfloat fov);
	void setSpeedMultiplicator(GLfloat val);

	void applyTransformation(Shader& shader, GLfloat width, GLfloat height) const;

private:
	glm::vec3 position;
	glm::vec3 xAxis;
	glm::vec3 yAxis;
	glm::vec3 zAxis;

	GLfloat pitch;
	GLfloat yaw;

	GLfloat nearRenderLimit;
	GLfloat farRenderLimit;
	GLfloat fov;

	GLfloat speed;
	GLfloat speedMultiplicator;
	GLfloat sensitivity;

	void setCoordinateSystem(const glm::vec3& zAxis);
	
	void move(Direction direction);
	void turn(GLfloat pitch, GLfloat yaw);

	void keyCallback(Window* window);
	void cursorPosCallback(Window* window);
	void scrollCallback(Window* window);
};
