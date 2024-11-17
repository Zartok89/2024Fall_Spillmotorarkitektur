#pragma once
#include <memory>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "Mesh.h"
#include <utility/RandomNumberGenerator.h>

class Material;

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
		DYNAMICOBJECT,
	};

	/*
	 * Actor Constructors and setup
	 */
	Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader, bool useTexture, const std::string& textureName);
	void ActorTransform();

	/*
	 * Getting transforms of the actor
	 */
	glm::mat4 SetActorTransform(glm::vec3 position, float scale);
	void SetActorPosition(glm::vec3 position);
	void SetActorCollision();
	void SetRandomActorVelocity();
	void SetActorVelocity(glm::vec3 actorVelocity) { mActorVelocity = actorVelocity; }
	void SetActorSpeed(float actorSpeed) { mActorSpeed = actorSpeed; }
	void SetActorMass(float actorMass) { mActorMass = actorMass; }

	/*
	 * Setting transforms of the actor
	 */
	glm::mat4 GetActorTransform() const { return mActorTransform; }
	glm::vec3 GetActorPosition() const { return mActorPosition; }
	float GetActorScale() const { return mActorScale; }
	glm::vec3 GetActorVelocity() const { return mActorVelocity; }
	float GetActorRadius() const { return mActorRadius; }
	float GetActorMass() const { return mActorMass; }

	/*
	 * Member Variables
	 */
	glm::vec3 mActorPosition{ 0.f };
	float mActorScale{ 1.f };
	std::string mName{ " " };
	std::string mTexture{ " " };
	bool mUseTexture{ false };
	std::shared_ptr<Material> mMaterial;
	ActorType mActorType{ STATIC };
	std::shared_ptr<Mesh> mMeshInfo;
	Shader* mShader;
	bool mNegativeDirection{ false };
	glm::vec3 mBoxExtendMin{ 0.f, 0.f, 0.f };
	glm::vec3 mBoxExtendMax{ 0.f, 0.f, 0.f };
	glm::vec3 mBoxExtendCenter{ 0.f, 0.f, 0.f };
	float mActorSpeed{ 20.f };

	// Pointers
	std::unique_ptr<RandomNumberGenerator> RandomNumberGenerator;

private:
	/*
	 * Private Member Variables
	 */
	glm::vec3 mActorRotationAxis{ 1.f, 0.f, 0.f };
	float mActorRotation{ 30.f };
	glm::mat4 mActorTransform{ 0.f };
	glm::vec3 mActorVelocity{ 0.f, 0.f, 0.f };
	float mActorMass{ 1.f };
	float mActorRadius{ 1.f };

};
