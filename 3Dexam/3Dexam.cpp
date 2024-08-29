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

/*
 * Application settings
 */
const unsigned int screenWidth = 800 * 2;
const unsigned int screenHeight = 600 * 2;
const char* windowTitle = "Exam Application";

void LeastSquaresMethodExample(std::shared_ptr<LeastSquaresMethod> leastSPtr)
{
	// LeastSquaresMethod example
	std::vector<Vertex> lsmVertices;
	constexpr glm::vec3 lsmVector1{ 1, -3, 1 };
	constexpr glm::vec3 lsmVector2{ 4, 5, 1 };
	constexpr glm::vec3 lsmVector3{ 8, -2, 1 };
	glm::vec3 LSMFunction = leastSPtr->LeastSquaresMethodAlgorithm(lsmVector1, lsmVector2, lsmVector3);
	leastSPtr->AddVertices(lsmVertices, -2.f, 5.f, .5f, LSMFunction);
}

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
	 * Mathmathical test functions
	 */
	//std::shared_ptr<LeastSquaresMethod> leastSPtr = std::make_shared<LeastSquaresMethod>();
	//LeastSquaresMethodExample(leastSPtr);

	/*
	 * Creating scene and loading in objects
	 */
	applicationPtr->scenePtr = std::make_shared<Scene>();
	applicationPtr->scenePtr->LoadScene();

	/*
	 * Creating camera and controllers and constructing pointers
	 */
	applicationPtr->controllerPtr = std::make_shared<Controller>(window, applicationPtr->scenePtr->mShader, screenWidth, screenHeight);
	applicationPtr->controllerPtr->cameraPtr = std::make_shared<Camera>(applicationPtr->scenePtr->mShader, glm::vec3{ 50.f, 100.f, 150.f }, -50.f, 8.f, 1.f, 45.f);
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