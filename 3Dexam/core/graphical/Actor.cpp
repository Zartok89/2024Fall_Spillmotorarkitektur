#include "Actor.h"
#include "graphical/Material.h"

// Constructor of an actor
Actor::Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position,
	glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType,
	Shader* shader, bool useTexture, const std::string& textureName = "")
	: mMeshInfo(meshInfo),
	mUseTexture(useTexture),
	mTexture(textureName),
	mActorPosition(position),
	mActorRotation(rotation),
	mActorRotationAxis(rotationAxis),
	mActorScale(scale),
	mShader(shader),
	mName(meshName),
	mActorType(actorType)

{
	ActorTransform();

	if (actorType == ActorType::BALL || actorType == ActorType::STATIC)
	{
		SetActorCollision();
	}

	if (actorType == ActorType::BALL)
	{
		mNegativeDirection = false;
		mActorVelocity = glm::vec3(0.f, 0.f, 0.f);
		mActorMass = 1.f;
		SetRandomActorVelocity();
	}
}

// Setup of the transform of the actor
void Actor::ActorTransform()
{
	glm::mat4 model{ 1.f };
	model = glm::translate(model, mActorPosition);
	model = glm::rotate(model, glm::radians(mActorRotation), mActorRotationAxis);
	model = glm::scale(model, glm::vec3{ mActorScale });
	mActorTransform = model;
}

glm::mat4 Actor::SetActorTransform(glm::vec3 position, float scale)
{
	glm::mat4 model{ 1.f };
	model = glm::translate(model, glm::vec3{ mActorPosition.x, mActorPosition.y, mActorPosition.z });
	model = glm::rotate(model, glm::radians(mActorRotation), mActorRotationAxis);
	model = glm::scale(model, glm::vec3{ scale });
	mActorTransform = model;
	mShader->setMat4("model", mActorTransform);
	return mActorTransform;
}

void Actor::SetActorPosition(glm::vec3 position)
{
	mActorPosition = position;
	SetActorTransform(mActorPosition, mActorScale);
}

void Actor::SetActorCollision()
{
	std::pair<glm::vec3, glm::vec3> BoxExtendPair = mMeshInfo->CalculateBoxExtent();
	mBoxExtendMin = BoxExtendPair.first;
	mBoxExtendMax = BoxExtendPair.second;
	mBoxExtendCenter = (glm::abs(mBoxExtendMin) + glm::abs(mBoxExtendMax)) / glm::vec3{ 2.f, 2.f, 2.f };
	mActorRadius = mMeshInfo->CalculateRadius();
}

void Actor::SetRandomActorVelocity()
{
	float divisor = 10.f;
	int R1 = RandomNumberGenerator->GeneratorRandomNumber(-100, 100);
	int R2 = RandomNumberGenerator->GeneratorRandomNumber(-100, 100);
	int R3 = RandomNumberGenerator->GeneratorRandomNumber(-100, 100);
	float random1 = (R1 / divisor);
	float random2 = (R2 / divisor);
	float random3 = (R3 / divisor);

	glm::vec3 randomDirection(random1, random2, random3);
	randomDirection = glm::normalize(randomDirection);

	mActorVelocity = randomDirection * mActorSpeed;
}
//
//// Constructor of an actor
//Actor::Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position,
//	glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType,
//	Shader* shader, bool useTexture, std::shared_ptr<Material> material, const std::string& textureName = "")
//	: mMeshInfo(meshInfo),
//	mUseTexture(useTexture),
//	mTexture(textureName),
//	mActorPosition(position),
//	mActorRotation(rotation),
//	mActorRotationAxis(rotationAxis),
//	mActorScale(scale),
//	mShader(shader),
//	mName(meshName),
//	mActorType(actorType),
//	mMaterial(material)
//
//{
//	ActorTransform();
//
//	if (actorType == ActorType::BALL || actorType == ActorType::STATIC)
//	{
//		SetActorCollision();
//	}
//
//	if (actorType == ActorType::BALL)
//	{
//		mNegativeDirection = false;
//		mActorVelocity = glm::vec3(0.f, 0.f, 0.f);
//		mActorMass = 1.f;
//		SetRandomActorVelocity();
//	}
//}