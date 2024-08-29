#pragma once
#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>

#include "graphical/Actor.h"
#include "graphical/Mesh.h"
#include "graphical/Texture.h"

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
	bool BarycentricCalculations(std::shared_ptr<Actor>& objectToCheck, glm::vec3 targetedPos, glm::vec3& newPositionVector);

	/*
	 * Member variables and unordered maps
	 */
	std::unordered_map<std::string, std::shared_ptr<Texture>> mSceneTextures;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> mSceneMeshes;
	std::unordered_map<std::string, std::shared_ptr<Actor>> mSceneActors;
	Shader* mShader = new Shader("core/shader/Shader.vs", "core/shader/Shader.fs");
	float mNpcSpeed{ 5.f };
	double previousTime = glfwGetTime();
	bool hasSetNewLine {false};
	// NPC Varibles
	int currentVertexIndex;
	bool movingForward;
	float interpolateFactor;
	float npcMovementSpeed;
};
