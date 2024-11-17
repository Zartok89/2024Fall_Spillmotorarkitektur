#pragma once
#include <chrono>
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
	void Update(float deltaTime);

	/*
	 * Loading
	 */
	void LoadScene();
	void LoadTextures();
	void LoadMeshes();
	//void LoadMaterials();
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
	bool NpcFollowCurve(float deltaTime, std::shared_ptr<Actor>& actors, std::string meshToFollow, std::string actorOffset);
	// Helper logic //
	glm::vec3 CalculateReflection(const glm::vec3& velocity, const glm::vec3& normal);

	/*
	 * Scene Physics
	 */
	void UpdateBall(std::shared_ptr<Actor>& objectToUpdate, float deltaTime, glm::vec3& normal);
	void CalculateAccelerationVector(glm::vec3& normal);
	void VelocityUpdate(std::shared_ptr<Actor>& objectToUpdate, float deltaTime);
	bool BarycentricCalculations(std::shared_ptr<Actor>& objectToCheck, glm::vec3 targetedPos, glm::vec3& newPositionVector, glm::vec3& normal);


	/*
	 * Member variables and unordered maps
	 */
	std::unordered_map<std::string, std::shared_ptr<Texture>> mSceneTextures;
	std::unordered_map<std::string, std::shared_ptr<Material>> mSceneMaterials;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> mSceneMeshes;
	std::unordered_map<std::string, std::shared_ptr<Actor>> mSceneActors;
	std::unordered_map<std::string, std::shared_ptr<Actor>> mSceneBallActors;
	Shader* mShader = new Shader("core/shader/Shader.vs", "core/shader/Shader.fs");
	float mNpcSpeed{ 5.f };
	//double previousTime = glfwGetTime();
	std::chrono::time_point<std::chrono::high_resolution_clock> previousTime;
    float deltaTime;
	bool hasSetNewLine{ false };
	bool shouldRenderWireframe{ false };

	/*CubeExtent & Octree*/
	void PopulateOctree();
	glm::vec3 minCubeExtent;
	glm::vec3 maxCubeExtent;
	int octreeCapacity = 8;

	/*NPC Varibles*/
	int currentVertexIndex;
	bool movingForward;
	float interpolateFactor;
	float npcMovementSpeed;

	/*Material variables*/
	glm::vec3 ambient {1.f, 1.f, 1.f};
	glm::vec3 diffuse {1.f, 1.f, 1.f};
	glm::vec3 specular {1.f, 1.f, 1.f};
	float shininess {1.0f};
		
	/*Physics Variables*/
	glm::vec3 mAcellerationVector;

	/*Pointers*/
	std::unique_ptr<RandomNumberGenerator> RandomNumberGenerator;
	std::unique_ptr<OctreeNode> OctreePtr;
};
