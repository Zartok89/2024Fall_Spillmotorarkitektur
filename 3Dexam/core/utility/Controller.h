#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include "application/Scene.h"
#include "graphical/Camera.h"

class Controller
{
public:
	enum ControllerType
	{
		NONE,
		CAMERA,
		PLAYER
	};

	/*
	 * Controller constructor
	 */
	Controller(GLFWwindow* window, Shader* shader, float screenWidth, float screenHeight);

	/*
	 * Controller inputs
	 */
	void CameraInputs(double dt);
	void PlayerInputs(double dt);
	void GeneralInputs(double dt);

	/*
	 * Member Variables and pointers
	 */
	GLFWwindow* mWindow;
	std::shared_ptr<Scene> scenePtr;
	std::shared_ptr<Camera> cameraPtr;
	// Matrixes
	float mRotationFloat{ 0 };
	glm::vec3 mPositionVector{ 0.0f, 0.0f, -3.f };
	glm::mat4 mProjection{ 1.f };
	glm::mat4 mView{ 1.f };
	// Controller variables
	float mPlayerSpeed{ 8.f };
	ControllerType controllerState{ CAMERA };
	bool bUsePlayerController = false;
	// Misc variables
	float mScreenWidth;
	float mScreenHeight;
	Shader* mShader;
	float FarPlane{ 500.f };
	float NearPlane{ 0.1f };
};
