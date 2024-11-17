#include "Application.h"
#include <iostream>
#include <memory>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void Application::InitializeOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool Application::InitializeWindow(GLFWwindow*& window, const char* windowTitle, int screenWidth, int screenHeight)
{
	window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return true;
	}

	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, this);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	InitializeCallbacks(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return true;
	}
	glEnable(GL_DEPTH_TEST);
	return false;
}

/*
 * ----Code is from: https://github.com/3Dprog24-Guest-Lecture---- *
 * All the callbacks to be used in the application
 */
void Application::InitializeCallbacks(GLFWwindow*& window)
{
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		if (app)
		{
			app->FramebufferSizeCallback(window, width, height);
		}
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		if (app) app->MouseMoveCallback(window, xpos, ypos);
		});

	//glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
	//	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	//	if (app) app->MouseButtonCallback(window, button, action, mods);
	//	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		if (app) app->MouseScrollCallback(window, xoffset, yoffset);
		});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		if (app) app->KeyCallback(window, key, scancode, action, mods);
		});

	//glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int c) {
	//	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	//	if (app) app->CharCallback(window, c);
	//	});
}

void Application::RunningApplication()
{
	glfwPollEvents();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Switching between player and camera controller
	if ((key == GLFW_KEY_TAB) && (action == GLFW_PRESS))
	{
		if (controllerPtr->bUsePlayerController)
		{
			controllerPtr->controllerState = Controller::CAMERA;
			controllerPtr->bUsePlayerController = false;
		}
		else
		{
			controllerPtr->controllerState = Controller::PLAYER;
			controllerPtr->bUsePlayerController = true;
		}
	}

	// Enable and disable wireframe
	if ((key == GLFW_KEY_F1) && (action == GLFW_PRESS))
	{
		if (!bIsWireframeEnabled)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			bIsWireframeEnabled = true;
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			bIsWireframeEnabled = false;
		}
	}

	// Closing the application
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Application::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!controllerPtr->bUsePlayerController)
	{
		controllerPtr->cameraPtr->ProcessMouseMovement(xpos, ypos);
	}
}

void Application::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	yoffset = 1.f;
	controllerPtr->cameraPtr->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Application::CheckingControllerState(float deltaTime)
{
	if (controllerPtr->controllerState == Controller::PLAYER)
	{
		controllerPtr->PlayerInputs(deltaTime);
	}
	if (controllerPtr->controllerState == Controller::CAMERA)
	{
		controllerPtr->CameraInputs(deltaTime);
	}
	controllerPtr->GeneralInputs(deltaTime);
}