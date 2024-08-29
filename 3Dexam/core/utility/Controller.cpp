#include "Controller.h"
#include <iostream>
#include <GLFW/glfw3.h>

#include "application/Application.h"
#include "graphical/Camera.h"

Controller::Controller(GLFWwindow* window, Shader* shader, float screenWidth, float screenHeight)
{
	mWindow = window;
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mShader = shader;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)mScreenWidth / (float)mScreenHeight, 0.1f, 100.0f);
	mShader->setMat4("projection", projection);
}

void Controller::CameraInputs(double dt)
{
	cameraPtr->updateCameraVectors();
	cameraPtr->ProcessMouseScroll(0.1f);
	mProjection = glm::perspective(glm::radians(45.0f), (float)mScreenWidth / (float)mScreenHeight, 0.1f, 100.0f);
	mShader->setMat4("projection", mProjection);

	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPtr->ProcessKeyboard(FORWARD, dt);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPtr->ProcessKeyboard(BACKWARD, dt);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPtr->ProcessKeyboard(LEFT, dt);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPtr->ProcessKeyboard(RIGHT, dt);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraPtr->ProcessKeyboard(UP, dt);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraPtr->ProcessKeyboard(DOWN, dt);
	}
	if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cameraPtr->mCameraSpeed = 20.f;
	}
	else if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		cameraPtr->mCameraSpeed = 8.f;
	}
}

void Controller::PlayerInputs(double dt)
{
	cameraPtr->updateCameraVectors();

	mProjection = glm::perspective(glm::radians(45.0f), (float)mScreenWidth / (float)mScreenHeight, 0.1f, 100.0f);

	mShader->setMat4("projection", mProjection);

	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		for (auto& actors : scenePtr->mSceneActors)
		{
			if (actors.second->mActorType == Actor::PLAYER)
			{
				float posZ = actors.second->mActorPosition.z -= mPlayerSpeed * dt;
				actors.second->SetActorTransform({ 0.f, 0.f, posZ }, actors.second->mActorScale);
			}
		}
	}

	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		for (auto& actors : scenePtr->mSceneActors)
		{
			if (actors.second->mActorType == Actor::PLAYER)
			{
				float posZ = actors.second->mActorPosition.z += mPlayerSpeed * dt;
				actors.second->SetActorTransform({ 0.f, 0.f, posZ }, actors.second->mActorScale);
			}
		}
	}

	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		for (auto& actors : scenePtr->mSceneActors)
		{
			if (actors.second->mActorType == Actor::PLAYER)
			{
				float posX = actors.second->mActorPosition.x -= mPlayerSpeed * dt;
				actors.second->SetActorTransform({ posX, 0.f, 0.f }, actors.second->mActorScale);
			}
		}
	}

	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		for (auto& actors : scenePtr->mSceneActors)
		{
			if (actors.second->mActorType == Actor::PLAYER)
			{
				float posX = actors.second->mActorPosition.x += mPlayerSpeed * dt;
				actors.second->SetActorTransform({ posX, 0.f, 0.f }, actors.second->mActorScale);
			}
		}
	}

	if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		mPlayerSpeed = 15.f;
	}
	else if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		mPlayerSpeed = 8.f;
	}
}