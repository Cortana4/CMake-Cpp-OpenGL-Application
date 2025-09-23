#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"


Camera::Camera(
	const glm::vec3& position,
	const glm::vec3& target,
	GLfloat nearRenderLimit,
	GLfloat farRenderLimit,
	GLfloat fov
)
	: position{ position }
	, pitch{ 0.0f }
	, yaw{ 0.0f }
	, nearRenderLimit{ nearRenderLimit }
	, farRenderLimit{ farRenderLimit }
	, fov{ fov }
	, speed{ 1.0f }
	, speedMultiplicator{ 4.0f }
	, sensitivity{ 0.1f }
{
	setTarget(target);
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position - zAxis, yAxis);
}

glm::mat4 Camera::getProjMatrix(GLfloat width, GLfloat height) const
{
	return glm::perspective(
		glm::radians(fov),
		width / height,
		nearRenderLimit,
		farRenderLimit
	);
}

const glm::vec3& Camera::getPosition() const
{
	return position;
}

const glm::vec3& Camera::getXAxis() const
{
	return xAxis;
}

const glm::vec3& Camera::getYAxis() const
{
	return yAxis;
}

const glm::vec3& Camera::getZAxis() const
{
	return zAxis;
}

GLfloat Camera::getNearRenderLimit() const
{
	return nearRenderLimit;
}

GLfloat Camera::getFarRenderLimit() const
{
	return farRenderLimit;
}

GLfloat Camera::getFov() const
{
	return fov;
}

std::function<void(Window*)> Camera::getKeyCallback()
{
	return std::bind(&Camera::keyCallback, this, std::placeholders::_1);
}

std::function<void(Window*)> Camera::getCursorPosCallback()
{
	return std::bind(&Camera::cursorPosCallback, this, std::placeholders::_1);
}

std::function<void(Window*)> Camera::getScrollCallback()
{
	return std::bind(&Camera::scrollCallback, this, std::placeholders::_1);
}

void Camera::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Camera::setTarget(const glm::vec3& target)
{
	setCoordinateSystem(position - target);

	pitch = glm::degrees(
		glm::half_pi<GLfloat>() - glm::angle(glm::vec3(0.0f, 1.0f, 0.0f), zAxis));
	yaw = glm::degrees(glm::angle(glm::vec3(1.0f, 0.0f, 0.0f), zAxis));
}

void Camera::setNearRenderLimit(GLfloat limit)
{
	this->nearRenderLimit = limit;
}

void Camera::setFarRenderLimit(GLfloat limit)
{
	this->farRenderLimit = limit;
}

void Camera::setFov(GLfloat fov)
{
	this->fov = fov;
}

void Camera::setSpeedMultiplicator(GLfloat val)
{
	speedMultiplicator = val;
}

void Camera::applyTransformation(Shader& shader, GLfloat width, GLfloat height) const
{
	shader.useProgram();
	shader.setUniformMatrix4fv("view", glm::value_ptr(getViewMatrix()));
	shader.setUniformMatrix4fv("projection", glm::value_ptr(getProjMatrix(width, height)));
}

void Camera::setCoordinateSystem(const glm::vec3& zAxis)
{
	this->zAxis = glm::normalize(zAxis);
	this->xAxis = glm::normalize(cross(glm::vec3(0.0f, 1.0f, 0.0f), this->zAxis));
	this->yAxis = glm::normalize(cross(this->zAxis, this->xAxis));
}

void Camera::move(Direction direction)
{
	switch (direction)
	{
	case FORWARD:
		position -= glm::normalize(glm::vec3(zAxis.x, 0.0f, zAxis.z)) * speed;
		break;

	case BACKWARD:
		position += glm::normalize(glm::vec3(zAxis.x, 0.0f, zAxis.z)) * speed;
		break;

	case LEFT:
		position -= xAxis * speed;
		break;

	case RIGHT:
		position += xAxis * speed;
		break;

	case UP:
		position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
		break;

	case DOWN:
		position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
		break;
	}
}

void Camera::turn(GLfloat pitch, GLfloat yaw)
{
	zAxis.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	zAxis.y = glm::sin(glm::radians(pitch));
	zAxis.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	
	setCoordinateSystem(zAxis);
}

void Camera::keyCallback(Window* window)
{
	speed = speedMultiplicator * static_cast<GLfloat>(window->getDeltaTime());

	if (window->getKeyStatus(GLFW_KEY_W))
		move(FORWARD);

	if (window->getKeyStatus(GLFW_KEY_S))
		move(BACKWARD);

	if (window->getKeyStatus(GLFW_KEY_A))
		move(LEFT);

	if (window->getKeyStatus(GLFW_KEY_D))
		move(RIGHT);

	if (window->getKeyStatus(GLFW_KEY_SPACE))
		move(UP);

	if (window->getKeyStatus(GLFW_KEY_LEFT_SHIFT))
		move(DOWN);

	if (window->getKeyStatus(GLFW_KEY_F3))
		setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Camera::cursorPosCallback(Window* window)
{
	GLfloat posX = static_cast<GLfloat>(window->getCursorPosX());
	GLfloat posY = static_cast<GLfloat>(window->getCursorPosY());

	static double lastX = posX;
	static double lastY = posY;

	GLfloat deltaX = posX - lastX;
	GLfloat deltaY = posY - lastY;

	lastX = posX;
	lastY = posY;

	yaw += deltaX * sensitivity;
	pitch += deltaY * sensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;

	else if (pitch < -89.0f)
		pitch = -89.0f;

	turn(pitch, yaw);
}

void Camera::scrollCallback(Window* window)
{
	GLfloat offset = static_cast<GLfloat>(window->getScrollOffsetY());

	if (fov >= 1.0f && fov <= 45.0f)
		fov -= offset;

	if (fov <= 1.0f)
		fov = 1.0f;

	else if (fov >= 45.0f)
		fov = 45.0f;
}
