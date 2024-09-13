#include "Actor.h"

// Constructor of an actor without textures
Actor::Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader)
{
	mUseTexture = false;
	mMeshInfo = meshInfo;
	if (ActorType::BOUNCINGBALL || ActorType::STATIC)
	{
		SetActorCollision();
	}
	if (ActorType::BOUNCINGBALL)
	{
		BallActorSetup(meshName, position, rotationAxis, rotation, scale, actorType, shader);
	}
	else
	{
		ActorSetup(meshName, position, rotationAxis, rotation, scale, actorType, shader);
	}
}

// Constructor of an actor with textures
Actor::Actor(const std::string& meshName, std::shared_ptr<Mesh> meshInfo, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader, const std::string& textureName)
{
	ActorSetup(meshName, position, rotationAxis, rotation, scale, actorType, shader, textureName);
	mUseTexture = true;
	mMeshInfo = meshInfo;
	if (ActorType::BOUNCINGBALL || ActorType::STATIC)
	{
		SetActorCollision();
	}
	if (ActorType::BOUNCINGBALL)
	{
		SetRandomActorVelocity();
	}
}

// Setting up actor information for the constructors
void Actor::ActorSetup(const std::string& meshName, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader, const std::string& textureName)
{
	mActorPosition = position;
	mActorRotation = rotation;
	mActorRotationAxis = rotationAxis;
	mActorScale = scale;
	mShader = shader;
	mName = meshName;
	mTexture = textureName;
	mActorType = actorType;
	ActorTransform();
}

void Actor::BallActorSetup(const std::string& meshName, glm::vec3 position, glm::vec3 rotationAxis, float rotation, float scale, ActorType actorType, Shader* shader, const std::string& textureName)
{
	mActorPosition = position;
	mActorRotation = rotation;
	mActorRotationAxis = rotationAxis;
	mActorScale = scale;
	mShader = shader;
	mName = meshName;
	mTexture = textureName;
	mActorType = actorType;
	mNegativeDirection = false;
	mActorVelocity = { 0.f, 0.f, 0.f };
	ActorTransform();
	SetRandomActorVelocity();
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
	int R1 = RandomNumberGenerator->GeneratorRandomNumber(1, 100);
	int R2 = RandomNumberGenerator->GeneratorRandomNumber(1, 100);
	int R3 = RandomNumberGenerator->GeneratorRandomNumber(1, 100);
	float random1 = (R1 / divisor);
	float random2 = (R2 / divisor);
	float random3 = (R3 / divisor);

    glm::vec3 randomDirection(random1, random2, random3);  
    randomDirection = glm::normalize(randomDirection);  

    float desiredSpeed = 5.0f;
    mActorVelocity = randomDirection * desiredSpeed;  
}

void Actor::SetActorVelocity(glm::vec3 velocity)
{
	mActorVelocity = velocity;
}
