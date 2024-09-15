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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	HandleSceneCollision(deltaTime);
}

// Loading all the textures, meshes and actors to be ready for rendering
// **running before the "while loop" of main()**
void Scene::LoadScene()
{
	glLineWidth(10.f);

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
	// Map Bounds
	mSceneActors["CubeContainer"] = (std::make_shared<Actor>("CubeMesh", mSceneMeshes["CubeMesh"], glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 50.f, Actor::ActorType::STATIC, mShader, "GrassTexture"));
	auto& mapBounds = mSceneActors["CubeContainer"];

	minCubeExtent = mapBounds->mBoxExtendMin * mapBounds->GetActorScale();
	maxCubeExtent = mapBounds->mBoxExtendMax * mapBounds->GetActorScale();

	// Calculate center and half-dimensions for the octree boundary
	glm::vec3 center = (minCubeExtent + maxCubeExtent) * 0.5f;
	glm::vec3 halfDimension = (maxCubeExtent - minCubeExtent) * 0.5f;

	// Generate Octree
	AABB sceneBoundary{ center, halfDimension };
	OctreePtr = std::make_unique<OctreeNode>(sceneBoundary, octreeCapacity);

	int AmountOfBalls = 100;
	glm::vec3 tempVec = glm::vec3{ 0.f, 0.f, 0.f };
	for (int i = 0; i <= AmountOfBalls; i++)
	{
		mSceneBallActors["SphereObject " + std::to_string(i)] = (std::make_shared<Actor>("SphereMesh", mSceneMeshes["SphereMesh"], tempVec, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, .1f, Actor::ActorType::BALL, mShader, "BlueTexture"));
		tempVec = RandomNumberGenerator->GeneratorRandomVector(0, 25);
		OctreePtr->Insert(mSceneBallActors["SphereObject " + std::to_string(i)]);
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
		break;

	case Actor::NPC:
		actor->mActorPosition.x += mNpcSpeed * deltaTime;
		if (actor->GetActorPosition().x >= 5 || actor->GetActorPosition().x <= -5)
		{
			mNpcSpeed *= -1.f;
		}
		actor->SetActorPosition(actor->GetActorPosition());
		break;

	case Actor::CURVETOFOLLOW:
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	case Actor::NPC_FOLLOWCURVE:
		if (!NpcFollowCurve(deltaTime, actor, "LineCurvedMesh", "LineCurve"))
		{
		}
		break;

	case Actor::NPC_FOLLOWLINE:
		if (!NpcFollowCurve(deltaTime, actor, "LineMesh", "LineTest"))
		{
		}
		break;

	case Actor::PLAYER:
		glm::vec3 playerHeight;
		if (BarycentricCalculations(mSceneActors["Terrain"], actor->GetActorPosition(), playerHeight))
		{
			actor->SetActorPosition(playerHeight);
		}
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	case Actor::BALL:
		UpdateBall(deltaTime, actor);
		break;

	default:
		break;
	}
}

void Scene::HandleSceneCollision(float deltaTime)
{
	if (Actor::BALL)
	{
		for (auto& ballPair : mSceneBallActors)
		{
			auto& ball = ballPair.second;
			glm::vec3 positionChange = ball->GetActorVelocity() * deltaTime;
			ball->SetActorPosition(ball->GetActorPosition() + positionChange);
		}

		//PopulateOctree();

		std::vector<CollisionInfo> collisions = DetectAllCollisions();

		for (const auto& actorPair : mSceneActors) {
			auto& actor = actorPair.second;
			if (actor->mActorType != Actor::BALL && actor->mActorType != Actor::STATIC) continue;

			// Define a query range based on actor's position and radius
			AABB queryRange
			{
				actor->GetActorPosition(),
				glm::vec3(actor->GetActorRadius())
			};

			std::vector<std::shared_ptr<Actor>> potentialColliders;

			//OctreePtr->Query(queryRange, potentialColliders);

			for (const auto& collision : collisions)
			{
				ResolveCollision(collision);
			}
		}
	}
}

std::vector<Scene::CollisionInfo> Scene::DetectAllCollisions()
{
	std::vector<CollisionInfo> collisions;

	// Detect sphere vs. wall collisions
	for (auto& ballPair : mSceneBallActors)
	{
		auto& ball = ballPair.second;
		glm::vec3 position = ball->GetActorPosition();
		float scale = ball->GetActorScale();
		float radius = ball->GetActorRadius();
		float scaledBallRadius = radius * scale;

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
			if (position[axis] + scaledBallRadius > maxExtents[axis])
			{
				CollisionInfo info;
				info.actorA = ball;
				info.actorB = nullptr;
				info.collisionNormal = wallNormals[2 * axis];
				info.penetrationDepth = (position[axis] + scaledBallRadius) - maxExtents[axis];
				collisions.emplace_back(info);
			}
			else if (position[axis] - scaledBallRadius < minExtents[axis])
			{
				CollisionInfo info;
				info.actorA = ball;
				info.actorB = nullptr;
				info.collisionNormal = wallNormals[2 * axis + 1];
				info.penetrationDepth = minExtents[axis] - (position[axis] - scaledBallRadius);
				collisions.emplace_back(info);
			}
		}
	}

	// Detect sphere vs. sphere collisions
	for (auto it = mSceneBallActors.begin(); it != mSceneBallActors.end(); ++it)
	{
		for (auto jt = std::next(it); jt != mSceneBallActors.end(); ++jt)
		{
			auto& ballA = it->second;
			auto& ballB = jt->second;

			glm::vec3 posA = ballA->GetActorPosition();
			glm::vec3 posB = ballB->GetActorPosition();
			float radiusA = ballA->GetActorRadius() * ballA->GetActorScale();
			float radiusB = ballB->GetActorRadius() * ballB->GetActorScale();

			glm::vec3 delta = posA - posB;
			float distance = glm::length(delta);
			float sumRadii = radiusA + radiusB;

			if (distance < sumRadii)
			{
				CollisionInfo info;
				info.actorA = ballA;
				info.actorB = ballB;
				info.collisionNormal = glm::normalize(delta);
				info.penetrationDepth = sumRadii - distance;
				collisions.emplace_back(info);
			}
		}
	}

	return collisions;
}

void Scene::ResolveCollision(const CollisionInfo& collision)
{
	if (collision.actorB == nullptr)
	{
		// Handle sphere vs. wall collision
		auto ball = collision.actorA;
		glm::vec3 velocity = ball->GetActorVelocity();
		glm::vec3 surfaceNormal = collision.collisionNormal;

		// Reflect the velocity
		glm::vec3 reflectedVelocity = CalculateReflection(velocity, surfaceNormal);
		ball->SetActorVelocity(reflectedVelocity);

		// Positional correction
		glm::vec3 position = ball->GetActorPosition();
		// Move the ball out of the wall based on the penetration depth
		ball->SetActorPosition(position + collision.collisionNormal * collision.penetrationDepth);
	}
	else
	{
		// Handle sphere vs. sphere collision
		auto ballA = collision.actorA;
		auto ballB = collision.actorB;

		glm::vec3 velocityA = ballA->GetActorVelocity();
		glm::vec3 velocityB = ballB->GetActorVelocity();
		glm::vec3 normal = collision.collisionNormal;

		// Calculate relative velocity
		glm::vec3 relativeVelocity = velocityA - velocityB;
		float velAlongNormal = glm::dot(relativeVelocity, normal);

		// Do not resolve if velocities are separating
		if (velAlongNormal > 0)
			return;

		// Restitution coefficient
		float restitution = 1.0f;

		// Calculate impulse scalar
		float massA = ballA->GetActorMass();
		float massB = ballB->GetActorMass();
		float impulseMagnitude = -(1.0f + restitution) * velAlongNormal / (1.0f / massA + 1.0f / massB);
		glm::vec3 impulse = impulseMagnitude * normal;

		// Update velocities
		glm::vec3 newVelocityA = velocityA + (impulse / massA);
		glm::vec3 newVelocityB = velocityB - (impulse / massB);
		ballA->SetActorVelocity(newVelocityA);
		ballB->SetActorVelocity(newVelocityB);

		// Positional correction to prevent sinking
		float penetration = collision.penetrationDepth;
		float correctionPercent = 0.5f; // Distribute equally
		glm::vec3 correction = (penetration / (massA + massB)) * correctionPercent * normal;

		glm::vec3 newPosA = ballA->GetActorPosition() + correction * massB;
		glm::vec3 newPosB = ballB->GetActorPosition() - correction * massA;

		ballA->SetActorPosition(newPosA);
		ballB->SetActorPosition(newPosB);
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

void Scene::PopulateOctree()
{
	OctreePtr->Clear();
	for (const auto& actorPair : mSceneBallActors)
	{
		auto& actor = actorPair.second;
		OctreePtr->Insert(actor);
	}
}	

void Scene::UpdateBall(float deltaTime, std::shared_ptr<Actor>& actor)
{
	glm::vec3 position = actor->GetActorPosition();
	glm::vec3 velocity = actor->GetActorVelocity();
	float scale = actor->GetActorScale();
	float radius = actor->GetActorRadius();

	float scaledballradius = radius * scale;

	glm::vec3 positionchange = velocity * deltaTime;
	glm::vec3 newposition = position + positionchange;

	actor->SetActorVelocity(velocity);
	actor->SetActorPosition(newposition);
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
	glm::vec3 speed = actor->GetActorVelocity();
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

//void MainOctTreeStruct::InitializeOctStruct(int AmountOfStructs, std::unordered_map<std::string, std::vector<Actor&>> OctActorStructRef)
//{
//	for (int i = 0; i <= AmountOfStructs; i++)
//	{
//		OctActorStructVector.emplace_back(OctActorStructRef["OctActor" + std::to_string(i)]);
//	}
//}