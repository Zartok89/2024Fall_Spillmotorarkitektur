#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include "Scene.h"

Scene::Scene() = default;

// Rendringering all the actors that should be contained in the scene, setting its texture and mesh
// **running in the "while loop" of main()**
void Scene::RenderScene()
{
	double currentTime = glfwGetTime();
	float deltaTime = (float)(currentTime - previousTime);
	previousTime = currentTime;

	for (auto& actors : mSceneActors)
	{
		// Checking if the actors is to be using texture or colors
		if (actors.second->mUseTexture == true)
		{
			mSceneTextures[actors.second->mTexture]->BindTextures();
		}
		mShader->setBool("useTexture", actors.second->mUseTexture);

		// Running the scene logic
		ActorSceneLogic(deltaTime, actors);

		// Rendering the meshes
		mSceneMeshes[actors.second->mName]->RenderMesh();
	}

	for (auto& actors : mSceneBallActors)
	{
		// Checking if the actors is to be using texture or colors
		if (actors.second->mUseTexture == true)
		{
			mSceneTextures[actors.second->mTexture]->BindTextures();
		}
		mShader->setBool("useTexture", actors.second->mUseTexture);

		// Running the scene logic
		ActorSceneLogic(deltaTime, actors);

		// Rendering the meshes
		mSceneMeshes[actors.second->mName]->RenderMesh();
	}
}

// Loading all the textures, meshes and actors to be ready for rendering
// **running before the "while loop" of main()**
void Scene::LoadScene()
{
	LoadTextures();
	LoadMeshes();
	LoadActors();
	LoadVariables();
}

// Texture loading, adding them into an unordered map
void Scene::LoadTextures()
{
	mSceneTextures["BlueTexture"] = std::make_shared<Texture>(1, "../assets/Bluebox.png");
	mSceneTextures["SkyTexture"] = std::make_shared<Texture>(1, "../assets/Daylight.png");
	mSceneTextures["GrassTexture"] = std::make_shared<Texture>(1, "../assets/Grass.png");
}

// Mesh loading, adding them into an unordered map
void Scene::LoadMeshes()
{
	mSceneMeshes["LineMesh"] = std::make_shared<Mesh>(MeshShape::LINE, mShader);
	mSceneMeshes["LineCurvedMesh"] = std::make_shared<Mesh>(MeshShape::LINECURVE, mShader);
	mSceneMeshes["TriangleMesh"] = std::make_shared<Mesh>(MeshShape::TRIANGLE, mShader);
	mSceneMeshes["SquareMesh"] = std::make_shared<Mesh>(MeshShape::SQUARE, mShader);
	mSceneMeshes["CubeMesh"] = std::make_shared<Mesh>(MeshShape::CUBE, mShader);
	mSceneMeshes["CubeMeshColor"] = std::make_shared<Mesh>(MeshShape::CUBECOLOR, mShader);
	mSceneMeshes["SphereMesh"] = std::make_shared<Mesh>(MeshShape::SPHERE, mShader);
	mSceneMeshes["FlatTerrainMesh"] = std::make_shared<Mesh>(MeshShape::TERRAIN_FLAT, mShader);
	mSceneMeshes["CurvedTerrainMesh"] = std::make_shared<Mesh>(MeshShape::TERRAIN_CURVED, mShader);
}

// Actor loading, adding them into a vector of actors
void Scene::LoadActors()
{
	//MapBounds
	mSceneActors["CubeContainer"] = (std::make_shared<Actor>("CubeMesh", mSceneMeshes["CubeMesh"], glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 50.f, Actor::ActorType::STATIC, mShader, "GrassTexture"));
	auto& mapBounds = mSceneActors["CubeContainer"];

	minCubeExtent = mapBounds->mBoxExtendMin * mapBounds->GetActorScale();
	maxCubeExtent = mapBounds->mBoxExtendMax * mapBounds->GetActorScale();

	int AmountOfBalls = 500;
	glm::vec3 tempVec = glm::vec3{ 0.f, 0.f, 0.f };
	for (int i = 0; i <= AmountOfBalls; i++)
	{
		mSceneBallActors["SphereObject " + std::to_string(i)] = (std::make_shared<Actor>("SphereMesh", mSceneMeshes["SphereMesh"], tempVec, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, .1f, Actor::ActorType::BOUNCINGBALL, mShader, "BlueTexture"));
		tempVec = RandomNumberGenerator->GeneratorRandomVector(0, 25);
	}
}

void Scene::LoadVariables()
{
	currentVertexIndex = 0;
	movingForward = true;
	interpolateFactor = 1.0f;
	npcMovementSpeed = 2.f;
}

void Scene::ActorSceneLogic(float deltaTime, std::unordered_map<std::string, std::shared_ptr<Actor>>::value_type& actorEntry)
{
	auto& actor = actorEntry.second;
	glm::mat4 transform;

	switch (actor->mActorType) {
	case Actor::STATIC:
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		//BoxAgainstBoxCollision(deltaTime, actor);
		break;

	case Actor::NPC:
		actor->mActorPosition.x += mNpcSpeed * deltaTime;
		if (actor->mActorPosition.x >= 5 || actor->mActorPosition.x <= -5)
		{
			mNpcSpeed *= -1.f;
		}
		actor->SetActorPosition(actor->mActorPosition);
		break;

	case Actor::CURVETOFOLLOW:
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	case Actor::NPC_FOLLOWCURVE:
		if (!NpcFollowCurve(deltaTime, actor, "LineCurvedMesh", "LineCurve"))
		{
			// Handle non-follow case if necessary
		}
		break;

	case Actor::NPC_FOLLOWLINE:
		if (!NpcFollowCurve(deltaTime, actor, "LineMesh", "LineTest"))
		{
			// Handle non-follow case if necessary
		}
		break;

	case Actor::PLAYER:
		glm::vec3 playerHeight;
		if (BarycentricCalculations(mSceneActors["Terrain"], actor->mActorPosition, playerHeight))
		{
			actor->SetActorPosition(playerHeight);
		}
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	case Actor::BOUNCINGBALL:
		BallAgainstBoxCollision(deltaTime, actor);
		BallAgainstBallCollision(deltaTime, actor);
		break;

	default:
		break;
	}
}

bool Scene::BarycentricCalculations(std::shared_ptr<Actor>& objectToCheck, glm::vec3 targetedPos, glm::vec3& newPositionVector)
{
	glm::vec3 targetPosition = glm::vec3(targetedPos.x, targetedPos.z, 0);
	for (int i = 0; i < objectToCheck->mMeshInfo->mIndices.size(); i += 3)
	{
		unsigned int P1{ objectToCheck->mMeshInfo->mIndices[i] };
		unsigned int P2{ objectToCheck->mMeshInfo->mIndices[i + 1] };
		unsigned int P3{ objectToCheck->mMeshInfo->mIndices[i + 2] };

		glm::vec3 P = glm::vec3(objectToCheck->mMeshInfo->mVertices[P1].mPosition.x, objectToCheck->mMeshInfo->mVertices[P1].mPosition.z, 0.f);
		glm::vec3 Q = glm::vec3(objectToCheck->mMeshInfo->mVertices[P2].mPosition.x, objectToCheck->mMeshInfo->mVertices[P2].mPosition.z, 0.f);
		glm::vec3 R = glm::vec3(objectToCheck->mMeshInfo->mVertices[P3].mPosition.x, objectToCheck->mMeshInfo->mVertices[P3].mPosition.z, 0.f);

		float areal = glm::length(glm::cross(Q - P, R - P));

		float U = (glm::cross(Q - targetPosition, R - targetPosition).z) / areal;
		float V = (glm::cross(R - targetPosition, P - targetPosition).z) / areal;
		float W = (glm::cross(P - targetPosition, Q - targetPosition).z) / areal;

		if (U >= 0 && V >= 0 && W >= 0)
		{
			newPositionVector = U * objectToCheck->mMeshInfo->mVertices[P1].mPosition + V * objectToCheck->mMeshInfo->mVertices[P2].mPosition + W * objectToCheck->mMeshInfo->mVertices[P3].mPosition;
			return true;
		}
	}

	return false;
}

glm::vec3 Scene::CalculateReflection(const glm::vec3& velocity, const glm::vec3& normal)
{
	glm::vec3 normalizedNormal = glm::normalize(normal);

	glm::vec3 reflection = velocity - 2.0f * glm::dot(velocity, normalizedNormal) * normalizedNormal;

	return reflection;
}

bool Scene::NpcFollowCurve(float deltaTime, std::shared_ptr<Actor>& actors, std::string meshToFollow, std::string actorOffset)
{
	std::vector<Vertex>& vertices = mSceneMeshes[meshToFollow]->mVertices;
	if (vertices.empty()) return true;

	int nextVertexIndex = currentVertexIndex;
	if (movingForward) {
		if (currentVertexIndex < vertices.size() - 1) {
			nextVertexIndex = currentVertexIndex + 1;
		}
		else {
			movingForward = false;
			nextVertexIndex = currentVertexIndex - 1;
		}
	}
	else {
		if (currentVertexIndex > 0) {
			nextVertexIndex = currentVertexIndex - 1;
		}
		else {
			movingForward = true;
			nextVertexIndex = currentVertexIndex + 1;
		}
	}

	glm::vec3 offsetPosition = mSceneActors[actorOffset]->GetActorPosition();
	glm::vec3 currentVertex = vertices[currentVertexIndex].mPosition;
	glm::vec3 nextVertex = vertices[nextVertexIndex].mPosition;

	interpolateFactor += deltaTime * npcMovementSpeed;

	glm::vec3 interpolatedPosition = glm::mix(currentVertex + offsetPosition, nextVertex + offsetPosition, interpolateFactor);

	actors->SetActorPosition(interpolatedPosition);

	if (interpolateFactor >= 1.0f)
	{
		currentVertexIndex = nextVertexIndex;
		interpolateFactor = 0.0f;
	}
	return false;
}

void Scene::BoxAgainstBoxCollision(float deltaTime, std::shared_ptr<Actor>& actor)
{
	glm::vec3 position = actor->GetActorPosition();
	glm::vec3 speed = actor->mActorVelocity;
	float scale = actor->GetActorScale();
	glm::vec3 boxExtendMax = actor->mBoxExtendMax;

	glm::vec3 scaledBoxExtendMax = boxExtendMax * scale;

	glm::vec3 positionChange;

	if (actor->mNegativeDirection)
	{
		positionChange = -speed * deltaTime;
	}
	else
	{
		positionChange = speed * deltaTime;
	}

	position += positionChange;

	bool hasCollided = actor->mNegativeDirection;

	if (position.x + scaledBoxExtendMax.x >= maxCubeExtent.x || position.y + scaledBoxExtendMax.y >= maxCubeExtent.y || position.z + scaledBoxExtendMax.z >= maxCubeExtent.z)
	{
		hasCollided = true;
	}

	if (position.x - scaledBoxExtendMax.x <= minCubeExtent.x || position.y - scaledBoxExtendMax.y <= minCubeExtent.y || position.z - scaledBoxExtendMax.z <= minCubeExtent.z)
	{
		hasCollided = false;
	}

	actor->SetActorPosition(position);
	actor->mNegativeDirection = hasCollided;
}

void Scene::BallAgainstBoxCollision(float deltaTime, std::shared_ptr<Actor>& actor)
{
	glm::vec3 position = actor->GetActorPosition();
	glm::vec3 velocity = actor->GetActorVelocity();
	float scale = actor->GetActorScale();
	float radius = actor->GetActorRadius();

	float scaledBallRadius = radius * scale;

	glm::vec3 positionChange = velocity * deltaTime;
	glm::vec3 newPosition = position + positionChange;

	glm::vec3 minExtents = minCubeExtent;
	glm::vec3 maxExtents = maxCubeExtent;

	glm::vec3 wallNormals[6] =
	{
		glm::vec3(-1.f,  0.f,  0.f), // +X face
		glm::vec3(1.f,  0.f,  0.f), // -X face
		glm::vec3(0.f, -1.f,  0.f), // +Y face
		glm::vec3(0.f,  1.f,  0.f), // -Y face
		glm::vec3(0.f,  0.f, -1.f), // +Z face
		glm::vec3(0.f,  0.f,  1.f)  // -Z face
	};

	for (int axis = 0; axis < 3; ++axis)
	{
		if (newPosition[axis] + scaledBallRadius > maxExtents[axis])
		{
			glm::vec3 wallNormal = wallNormals[2 * axis];
			velocity = CalculateReflection(velocity, wallNormal);
			newPosition[axis] = maxExtents[axis] - scaledBallRadius;
		}
		else if (newPosition[axis] - scaledBallRadius < minExtents[axis])
		{
			glm::vec3 wallNormal = wallNormals[2 * axis + 1];
			velocity = CalculateReflection(velocity, wallNormal);
			newPosition[axis] = minExtents[axis] + scaledBallRadius;
		}
	}

	actor->SetActorVelocity(velocity);
	actor->SetActorPosition(newPosition);
}

void Scene::BallAgainstBallCollision(float deltaTime, std::shared_ptr<Actor>& actor)
{
	 // Retrieve the current ball's properties  
    glm::vec3 position = actor->GetActorPosition();  
    glm::vec3 velocity = actor->mActorVelocity;  
    float scale = actor->GetActorScale();  
    float radius = actor->GetActorRadius();  

    float scaledBallRadius = radius * scale;  

    // Calculate the new position based on velocity and deltaTime  
    glm::vec3 positionChange = velocity * deltaTime;  
    glm::vec3 newPosition = position + positionChange;  

    // Restitution coefficient (1 for elastic collision)  
    float restitution = 1.0f;  

    bool hasCollided = false;  

    // Iterate through all other balls in the scene  
    for (auto& ballPair : mSceneBallActors)  
    {  
        std::shared_ptr<Actor> otherBall = ballPair.second;  

        // Avoid checking collision with itself  
        if (otherBall.get() == actor.get())  
            continue;  

        glm::vec3 otherBallPosition = otherBall->GetActorPosition();  
        glm::vec3 otherBallVelocity = otherBall->mActorVelocity;  
        float otherBallScaledRadius = otherBall->mActorRadius * otherBall->GetActorScale();  

        // Calculate the vector between the two centers  
        glm::vec3 delta = newPosition - otherBallPosition;  
        float distance = glm::length(delta);  
        float sumRadii = scaledBallRadius + otherBallScaledRadius;  

        // Check for collision  
        if (distance < sumRadii)  
        {  
            hasCollided = true;  

            // Normalize the delta vector to get the surface normal  
            glm::vec3 surfaceNormal = glm::normalize(delta);  

            // Calculate relative velocity  
            glm::vec3 relativeVelocity = velocity - otherBallVelocity;  

            // Calculate velocity along the normal  
            float velAlongNormal = glm::dot(relativeVelocity, surfaceNormal);  

            // Do not resolve if velocities are separating  
            if (velAlongNormal > 0)  
                continue;  

            // Calculate impulse scalar  
            float massA = actor->GetActorMass();  
            float massB = otherBall->GetActorMass();  
            float impulseMagnitude = -(1.0f + restitution) * velAlongNormal / (1.0f/massA + 1.0f/massB);  
            glm::vec3 impulse = impulseMagnitude * surfaceNormal;  

            // Update velocities based on impulse  
            velocity += (impulse / massA);  
            otherBallVelocity -= (impulse / massB);  

            // Apply the updated velocities  
            actor->mActorVelocity = velocity;  
            otherBall->SetActorVelocity(otherBallVelocity);  

            // Positional correction to prevent sinking  
            float penetration = sumRadii - distance;  
            float correctionPercent = 0.5f; // Distribute correction equally  
            glm::vec3 correction = (penetration / (massA + massB)) * correctionPercent * surfaceNormal;  

            newPosition += correction * massB;  
            glm::vec3 correctedOtherPosition = otherBallPosition - correction * massA;  

            // Update positions  
            actor->SetActorPosition(newPosition);  
            otherBall->SetActorPosition(correctedOtherPosition);  
        }  
    }  

    // Update the actor's position  
    actor->SetActorPosition(newPosition);  

    // Update collision flag (optional, based on your specific needs)  
    actor->mNegativeDirection = hasCollided;  
}

//void MainOctTreeStruct::InitializeOctStruct(int AmountOfStructs, std::unordered_map<std::string, std::vector<Actor&>> OctActorStructRef)
//{
//	for (int i = 0; i <= AmountOfStructs; i++)
//	{
//		OctActorStructVector.emplace_back(OctActorStructRef["OctActor" + std::to_string(i)]);
//	}
//}