#include "Camera.h"

#include <iomanip>
#include <iostream>
#include <glm/fwd.hpp>

// Camera Constructor with vectors
Camera::Camera(Shader* shader, glm::vec3 position, float rotation, float cameraSpeed, float mouseSensitivity, float zoom)
{
	mCameraFront = (glm::vec3(0.0f, 0.0f, -1.0f));
	mCameraSpeed = cameraSpeed;
	mMouseSensitivity = mouseSensitivity;
	mZoom = zoom;
	mCameraPosition = position;
	mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	mShader = shader;
	mPitch = rotation;
	updateCameraVectors();
}

// Camera Constructor with scalars
Camera::Camera(Shader* shader, float posX, float posY, float posZ, float upX, float upY, float upZ, float cameraSpeed, float mouseSensitivity, float zoom)
{
	mCameraPosition = glm::vec3(posX, posY, posZ);
	mCameraFront = (glm::vec3(0.0f, 0.0f, -1.0f));
	mCameraSpeed = cameraSpeed;
	mMouseSensitivity = mouseSensitivity;
	mZoom = zoom;
	mWorldUp = glm::vec3(upX, upY, upZ);
	mShader = shader;
	updateCameraVectors();
}

void Camera::SetCameraPosition(glm::vec3 cameraPosition)
{
	mCameraPosition = cameraPosition;
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

	mCameraFront = glm::normalize(front);
	mCameraRightVector = glm::normalize(glm::cross(mCameraFront, mWorldUp));
	mCameraUpVector = glm::normalize(glm::cross(mCameraRightVector, mCameraFront));

	mView = glm::mat4(1.0f);
	mView = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mWorldUp);

	mShader->setMat4("view", mView);
}

void Camera::ProcessMouseScroll(float yoffset)
{
	mZoom -= (float)yoffset;
	if (mZoom < 1.0f)
		mZoom = 1.0f;
	if (mZoom > 45.0f)
		mZoom = 45.0f;
}

void Camera::ProcessMouseMovement(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	mYaw += xoffset;
	mPitch += yoffset;

	if (mPitch > 89.0f)
		mPitch = 89.0f;
	if (mPitch < -89.0f)
		mPitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	direction.y = sin(glm::radians(mPitch));
	direction.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mCameraFront = glm::normalize(direction);

	updateCameraVectors();
}

void Camera::ProcessKeyboard(CameraDirections direction, float deltaTime)
{
	float velocity = mCameraSpeed * deltaTime;

	if (direction == FORWARD)
	{
		mCameraPosition += mCameraFront * velocity;
	}
	if (direction == BACKWARD)
	{
		mCameraPosition -= mCameraFront * velocity;
	}
	if (direction == LEFT)
	{
		mCameraPosition -= mCameraRightVector * velocity;
	}
	if (direction == RIGHT)
	{
		mCameraPosition += mCameraRightVector * velocity;
	}
	if (direction == UP)
	{
		mCameraPosition += mWorldUp * velocity;
	}
	if (direction == DOWN)
	{
		mCameraPosition -= mWorldUp * velocity;
	}
}