#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <memory>
#include <stb/stb_image.h>

#include "shader/Shader.h"
#include "application/Application.h"
#include "application/Scene.h"
#include "graphical/Camera.h"
#include "graphical/Mesh.h"
#include "utility/Controller.h"
#include "utility/MathLibrary.h"
#include "utility/ReadWriteFiles.h"

/*
 * Application settings
 */
const unsigned int screenWidth = 800 * 2;
const unsigned int screenHeight = 600 * 2;
const char* windowTitle = "OpenGLFall2024";

int main()
{
	/*
	 * Initializing OpenGL and Window through application
	 */
	std::unique_ptr<Application> applicationPtr = std::make_unique<Application>();
	applicationPtr->InitializeOpenGL();
	GLFWwindow* window;
	applicationPtr->InitializeWindow(window, windowTitle, screenWidth, screenHeight);
	double previousTime = glfwGetTime();

	 /*
	  * Creating scene and loading in objects
	  */
	applicationPtr->scenePtr = std::make_shared<Scene>();
	applicationPtr->scenePtr->LoadScene();

	/*
	 * Creating camera and controllers and constructing pointers
	 */
	applicationPtr->controllerPtr = std::make_shared<Controller>(window, applicationPtr->scenePtr->mShader, screenWidth, screenHeight);
	applicationPtr->controllerPtr->cameraPtr = std::make_shared<Camera>(applicationPtr->scenePtr->mShader, glm::vec3{ 13.1661f, 417.759f, 259.571f }, -50.f, 30.f, 1.f, 45.f);
	applicationPtr->controllerPtr->scenePtr = applicationPtr->scenePtr;
	applicationPtr->controllerPtr->cameraPtr->lastX = screenWidth / 2.0f;
	applicationPtr->controllerPtr->cameraPtr->lastY = screenHeight / 2.0f;

	/*
	 * Application running-loop
	 */
	while (!glfwWindowShouldClose(window))
	{
		// Deltatime calculations
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - previousTime);
		previousTime = currentTime;

		// Running the application and setting up
		applicationPtr->RunningApplication();

		// Cheking if the controller is player or camera
		applicationPtr->CheckingControllerState(deltaTime);

		// Rendering the scene
		applicationPtr->scenePtr->RenderScene();

		glfwSwapBuffers(window);
	}

	return 0;
}