#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphical/Camera.h"
#include "utility/Controller.h"

class Application
{
public:

	/*
	 * Constructing and running the application
	 */
	void InitializeOpenGL();
	bool InitializeWindow(GLFWwindow*& window, const char* windowTitle, int screenWidth, int screenHeight);
	void InitializeCallbacks(GLFWwindow*& window);
	void RunningApplication();

	/*
	 * Seting up callbacks
	 */
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void CheckingControllerState(float deltaTime);

	/*
	 * Pointers and member variables
	 */
	std::shared_ptr<Controller> controllerPtr;
	std::shared_ptr<Scene> scenePtr;
	bool bIsWireframeEnabled{ false };
};