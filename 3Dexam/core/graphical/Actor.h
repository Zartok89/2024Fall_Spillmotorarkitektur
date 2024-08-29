#pragma once
#include <memory>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "Mesh.h"

class Actor
{
public:
	enum ActorType
	{
		STATIC,
		PICKUP,
		NPC,
		NPC_FOLLOWCURVE,
		NPC_FOLLOWLINE,
		CURVETOFOLLOW,
		PLAYER
	};

	/*
	 * Actor Constructors and setup
	 */
	Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader);
	Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader, const std::string& textureName);
	void ActorSetup(const std::string& meshName, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader, const std::string& textureName = "");
	void ActorTransform();

	/*
	 * Getting transforms of the actor
	 */
	glm::mat4 SetActorTransform(glm::vec3 position, float scale);
	void SetActorPosition(glm::vec3 position);

	/*
	 * Setting transforms of the actor
	 */
	glm::mat4 GetActorTransform() const { return mActorTransform; }
	glm::vec3 GetActorPosition() const { return mActorPosition; }

	/*
	 * Member Variables
	 */
	glm::vec3 mActorPosition{ 0.f };
	float mActorRotation{ 30.f };
	glm::vec3 mActorRotationAxis{ 1.f, 0.f, 0.f };
	glm::mat4 mActorTransform{ 0.f };
	float mActorScale{ 1.f };
	std::string mName{ " " };
	std::string mTexture{ " " };
	bool mUseTexture{ false };
	ActorType mActorType{ STATIC };
	std::shared_ptr<Mesh> mMeshInfo;
	Shader* mShader;
};