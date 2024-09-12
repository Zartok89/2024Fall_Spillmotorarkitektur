#pragma once
#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <iostream>

#include "utility/RandomNumberGenerator.h"
#include "graphical/Actor.h"
#include "graphical/Mesh.h"
#include "graphical/Texture.h"

class memory;

class Scene
{
public:

	/*
	 * Constructor of the Scene Class and rendering
	 */
	Scene();
	void RenderScene();

	/*
	 * Loading
	 */
	void LoadScene();
	void LoadTextures();
	void LoadMeshes();
	void LoadActors();
	void LoadVariables();

	/*
	 * Scene logic
	 */
	void ActorSceneLogic(float deltaTime, std::unordered_map<std::string, std::shared_ptr<Actor>>::value_type& actors);
	bool NpcFollowCurve(float deltaTime, std::shared_ptr<Actor>& actors, std::string meshToFollow, std::string actorOffset);
	void BallBouncingAround(float deltaTime, std::shared_ptr<Actor>& actor);
	bool BarycentricCalculations(std::shared_ptr<Actor>& objectToCheck, glm::vec3 targetedPos, glm::vec3& newPositionVector);

	/*
	 * Member variables and unordered maps
	 */
	std::unordered_map<std::string, std::shared_ptr<Texture>> mSceneTextures;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> mSceneMeshes;
	std::unordered_map<std::string, std::shared_ptr<Actor>> mSceneActors;
	std::unordered_map<std::string, std::shared_ptr<Actor>> mSceneBallActors;
	Shader* mShader = new Shader("core/shader/Shader.vs", "core/shader/Shader.fs");
	float mNpcSpeed{ 5.f };
	double previousTime = glfwGetTime();
	bool hasSetNewLine{ false };

	//CubeExtent
	glm::vec3 minCubeExtent;
	glm::vec3 maxCubeExtent;

	// NPC Varibles
	int currentVertexIndex;
	bool movingForward;
	float interpolateFactor;
	float npcMovementSpeed;

	// Pointers
	std::unique_ptr<RandomNumberGenerator> RandomNumberGenerator;
};
