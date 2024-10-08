#pragma once
#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <iostream>

#include "utility/RandomNumberGenerator.h"
#include "graphical/Actor.h"
#include "graphical/Mesh.h"
#include "graphical/Texture.h"
#include "utility/Octree.h"

class memory;

class Scene
{
	struct CollisionInfo
	{
		std::shared_ptr<Actor> actorA;
		std::shared_ptr<Actor> actorB;
		glm::vec3 collisionNormal;
		float penetrationDepth;
	};

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
	// Collision logic //
	void HandleSceneCollision(float deltaTime);
	std::vector<CollisionInfo> DetectAllCollisions();
	void ResolveCollision(const CollisionInfo& collision);
	void BoxAgainstBoxCollision(float deltaTime, std::shared_ptr<Actor>& actor);
	// Pathing logic //
	void UpdateBall(float deltaTime, std::shared_ptr<Actor>& actor);
	bool NpcFollowCurve(float deltaTime, std::shared_ptr<Actor>& actors, std::string meshToFollow, std::string actorOffset);
	bool BarycentricCalculations(std::shared_ptr<Actor>& objectToCheck, glm::vec3 targetedPos, glm::vec3& newPositionVector);
	// Helper logic //
	glm::vec3 CalculateReflection(const glm::vec3& velocity, const glm::vec3& normal);


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

	// CubeExtent & Octree
	void PopulateOctree();
	glm::vec3 minCubeExtent;
	glm::vec3 maxCubeExtent;
	int octreeCapacity = 8;

	// NPC Varibles
	int currentVertexIndex;
	bool movingForward;
	float interpolateFactor;
	float npcMovementSpeed;

	// Pointers
	std::unique_ptr<RandomNumberGenerator> RandomNumberGenerator;
	std::unique_ptr<OctreeNode> OctreePtr;
};
