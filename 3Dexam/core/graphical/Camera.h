#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>

#include "shader/Shader.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraDirections {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	/*
	 * Camera constructors and update
	 */
	Camera(Shader* shader, glm::vec3 position, float rotation, float cameraSpeed, float mouseSensitivity, float zoom);
	Camera(Shader* shader, float posX, float posY, float posZ, float upX, float upY, float upZ, float cameraSpeed, float mouseSensitivity, float zoom);
	void updateCameraVectors();

	/*
	 * Getter vector and transform functions
	 */
	glm::vec3 GetRightVector() const { return glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), mCameraDirection)); }
	glm::vec3 GetUpVector() const { return glm::cross(mCameraDirection, mCameraRightVector); }
	glm::vec3 GetCameraPosition() const { return mCameraPosition; }
	glm::mat4 GetViewMatrix() { return glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mWorldUp); }

	/*
	 * Setter vector and transform functions
	 */
	void SetCameraPosition(glm::vec3 cameraPosition);

	/*
	 * Handling inputs
	 */
	void ProcessMouseScroll(float yoffset);
	void ProcessMouseMovement(double xpos, double ypos);
	void ProcessKeyboard(CameraDirections direction, float deltaTime);

	/*
	 * Member variables
	 */
	 // Camera position and vectors
	glm::vec3 mCameraPosition{ glm::vec3(0.f, 0.f, 0.f) };
	glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 mCameraTarget{ glm::vec3(0.f, 0.f, 0.f) };
	glm::vec3 mCameraDirection = glm::normalize(mCameraPosition - mCameraTarget);
	glm::vec3 mCameraRightVector = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), mCameraDirection));
	glm::vec3 mCameraUpVector = glm::normalize(glm::cross(mCameraRightVector, mCameraFront));
	// Euler angles
	float mYaw{ -90.0f };
	float mPitch{ 0.0f };
	// Camera options
	float mCameraSpeed{ 50.f };
	float mMouseSensitivity{ 0.1f };
	float mZoom{ 45.0f };
	// Default matrix for the camera
	glm::mat4 mModel = glm::rotate(glm::mat4{ 1.f }, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mProjection{ 1.f };
	glm::mat4 mView{ 1.f };
	// Misc pointer and variables
	Shader* mShader;
	float lastX;
	float lastY;
	bool firstMouse = true;
};
