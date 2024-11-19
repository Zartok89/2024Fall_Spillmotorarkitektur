#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <glm/glm.hpp>
#include "Scene.h"
#include "graphical/Material.h"

Scene::Scene()
{
	previousTime = std::chrono::high_resolution_clock::now();
}
// Rendringering all the actors that should be contained in the scene, setting its texture and mesh
// **running in the "while loop" of main()**
void Scene::RenderScene()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsedTime = currentTime - previousTime;
	deltaTime = elapsedTime.count();
	previousTime = currentTime;

	// Clamp delta time to a maximum value
	const float maxDeltaTime = 0.1f;
	if (deltaTime > maxDeltaTime)
	{
		deltaTime = maxDeltaTime;
	}

	// Spline update timer
	splineTimer += deltaTime;
	if (splineTimer > 1)
	{
		splineTimer = 0;
		timerEnabled = true;
	}

	Update(deltaTime);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (auto& actors : mSceneActors)
	{
		mSceneMeshes[actors.second->mName]->mMeshShader->use();

		if (actors.second->mUseTexture == true)
		{
			glActiveTexture(GL_TEXTURE0);
			mSceneTextures[actors.second->mTexture]->BindTextures();
			mShader->setInt("texture1", 0);
		}
		mShader->setBool("useTexture", actors.second->mUseTexture);

		ActorSceneLogic(deltaTime, actors);

		shouldRenderWireframe ? mSceneMeshes[actors.second->mName]->setWireframe = true : mSceneMeshes[actors.second->mName]->setWireframe = false;

		mSceneMeshes[actors.second->mName]->RenderMesh();
	}

	for (auto& actors : mSceneBallActors)
	{
		mSceneMeshes[actors.second->mName]->mMeshShader->use();

		if (actors.second->mUseTexture == true)
		{
			glActiveTexture(GL_TEXTURE0);
			mSceneTextures[actors.second->mTexture]->BindTextures();
			mShader->setInt("texture1", 0);
		}
		mShader->setBool("useTexture", actors.second->mUseTexture);

		ActorSceneLogic(deltaTime, actors);

		shouldRenderWireframe ? mSceneMeshes[actors.second->mName]->setWireframe = true : mSceneMeshes[actors.second->mName]->setWireframe = false;

		mSceneMeshes[actors.second->mName]->RenderMesh();
	}

	for (auto& actors : mBSplineActors)
	{
		mSceneMeshes[actors.second->mName]->mMeshShader->use();

		if (actors.second->mUseTexture == true)
		{
			glActiveTexture(GL_TEXTURE0);
			mSceneTextures[actors.second->mTexture]->BindTextures();
			mShader->setInt("texture1", 0);
		}
		mShader->setBool("useTexture", actors.second->mUseTexture);

		ActorSceneLogic(deltaTime, actors);

		shouldRenderWireframe ? mSceneMeshes[actors.second->mName]->setWireframe = true : mSceneMeshes[actors.second->mName]->setWireframe = false;

		mSceneMeshes[actors.second->mName]->RenderMesh();
	}

	// Handling the scene collision
	HandleSceneCollision(deltaTime);

	timerEnabled = false;
}

void Scene::Update(float deltaTime)
{
	for (auto& actors : mSceneActors)
	{
		ActorSceneLogic(deltaTime, actors);
	}

	for (auto& actors : mSceneBallActors)
	{
		ActorSceneLogic(deltaTime, actors);
	}

	for (auto& actors : mBSplineActors)
	{
		ActorSceneLogic(deltaTime, actors);
	}
}

// Loading all the textures, meshes and actors to be ready for rendering
// **running before the "while loop" of main()**
void Scene::LoadScene()
{
	LoadTextures();
	LoadMeshes();
	LoadActors();
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
	mSceneMeshes["PunktSkyMesh"] = std::make_shared<Mesh>(MeshShape::PUNKTSKY, mShader);
	mSceneMeshes["BSplineMesh"] = std::make_shared<Mesh>(MeshShape::BSPLINE, mShader);
}

// Actor loading, adding them into a vector of actors
void Scene::LoadActors()
{
	/*Terrain*/
	mSceneActors["PunktSky"] = (std::make_shared<Actor>("PunktSkyMesh", mSceneMeshes["PunktSkyMesh"], glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::STATIC, mShader, false, ""));
	minTerrainLimit = mSceneActors["PunktSky"]->mMeshInfo->minTerrainLimit;
	maxTerrainLimit = mSceneActors["PunktSky"]->mMeshInfo->maxTerrainLimit;
	CustomArea = mSceneActors["PunktSky"]->mMeshInfo->customArea;
}

void Scene::ActorSceneLogic(float deltaTime, std::unordered_map<std::string, std::shared_ptr<Actor>>::value_type& actors)
{
	auto& actor = actors.second;
	glm::mat4 transform;

	switch (actor->mActorType)
	{
	case Actor::STATIC:
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	case Actor::DYNAMICOBJECT:
		glm::vec3 objectHeight;
		glm::vec3 objectNormal;
		if (shouldSimualtePhysics)
		{
			if (BarycentricCalculations(mSceneActors["PunktSky"], actor->GetActorPosition(), objectHeight, objectNormal))
			{
				actor->SetActorPosition(objectHeight);
				ObjectPhysics(actor, deltaTime, objectNormal);
				//std::cout << actor->GetActorPosition().x << ", " << actor->GetActorPosition().y << ", " << actor->GetActorPosition().z << "\n";
			}
		}
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	case Actor::SPLINE:
		// Drawing the B-spline curve
		DrawBSplineCurve(actor);
		transform = actor->GetActorTransform();
		mShader->setMat4("model", transform);
		break;

	default:
		break;
	}
}

void Scene::HandleSceneCollision(float deltaTime)
{
	if (Actor::DYNAMICOBJECT)
	{
		std::vector<CollisionInfo> collisions = DetectAllCollisions();

		for (const auto& collision : collisions)
		{
			ResolveCollision(collision);
		}
	}
}

std::vector<Scene::CollisionInfo> Scene::DetectAllCollisions()
{
	std::vector<CollisionInfo> collisions;

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

bool Scene::BarycentricCalculations(std::shared_ptr<Actor>& objectToCheck, glm::vec3 targetedPos, glm::vec3& newPositionVector, glm::vec3& normal)
{
	glm::vec3 targetPosition = glm::vec3(targetedPos.x, targetedPos.z, 0);

	for (size_t i = 0; i < objectToCheck->mMeshInfo->mIndices.size(); i += 3)
	{
		int P1 = objectToCheck->mMeshInfo->mIndices[i];
		int P2 = objectToCheck->mMeshInfo->mIndices[i + 1];
		int P3 = objectToCheck->mMeshInfo->mIndices[i + 2];

		glm::vec3 P = glm::vec3(objectToCheck->mMeshInfo->mVertices[P1].mPosition.x, objectToCheck->mMeshInfo->mVertices[P1].mPosition.z, 0.f);
		glm::vec3 Q = glm::vec3(objectToCheck->mMeshInfo->mVertices[P2].mPosition.x, objectToCheck->mMeshInfo->mVertices[P2].mPosition.z, 0.f);
		glm::vec3 R = glm::vec3(objectToCheck->mMeshInfo->mVertices[P3].mPosition.x, objectToCheck->mMeshInfo->mVertices[P3].mPosition.z, 0.f);

		glm::vec3 PQ = Q - P;
		glm::vec3 PR = R - P;
		float area = glm::length(glm::cross(PQ, PR));

		float U = glm::cross(Q - targetPosition, R - targetPosition).z / area;
		float V = glm::cross(R - targetPosition, P - targetPosition).z / area;
		float W = glm::cross(P - targetPosition, Q - targetPosition).z / area;

		// Use a small tolerance to handle floating-point precision issues
		const float tolerance = 1e-5f;
		if (U >= -tolerance && V >= -tolerance && W >= -tolerance && std::abs(U + V + W - 1.0f) <= tolerance)
		{
			// Calculate the new Y position using barycentric coordinates
			float newY = U * objectToCheck->mMeshInfo->mVertices[P1].mPosition.y +
				V * objectToCheck->mMeshInfo->mVertices[P2].mPosition.y +
				W * objectToCheck->mMeshInfo->mVertices[P3].mPosition.y;

			// Set the new position with the correct Y value
			newPositionVector = glm::vec3(targetedPos.x, newY + 1.f, targetedPos.z);

			// Calculate the normal of the triangle
			glm::vec3 normalP = objectToCheck->mMeshInfo->mVertices[P1].mNormal;
			glm::vec3 normalQ = objectToCheck->mMeshInfo->mVertices[P2].mNormal;
			glm::vec3 normalR = objectToCheck->mMeshInfo->mVertices[P3].mNormal;
			normal = glm::normalize(U * normalP + V * normalQ + W * normalR);

			return true;
		}
	}
	return false;
}

void Scene::FrictionUpdate(std::shared_ptr<Actor>& objectToUpdate, float deltaTime, const glm::vec3& normal)
{
	// Friction coefficients
	const float mu = CustomArea[0].areaFriction;

	// Get porperties of the current object
	glm::vec3 velocity = objectToUpdate->GetActorVelocity();
	float mass = objectToUpdate->GetActorMass();

	// Setting gravity
	const float g = 0.981f * 3.f;
	glm::vec3 gravity(0.0f, -mass * g, 0.0f);

	// Calculate the normal force
	glm::vec3 normalizedNormal = glm::normalize(normal);
	float normalForceMagnitude = glm::dot(gravity, normalizedNormal);

	// Calculate the friction force
	glm::vec3 frictionForce;
	glm::vec3 frictionDirection = -glm::normalize(velocity);
	frictionForce = mu * normalForceMagnitude * frictionDirection;

	// Update the velocity based on the friction force
	glm::vec3 accelerationDueToFriction = frictionForce / mass;
	glm::vec3 updatedVelocity = velocity + accelerationDueToFriction * deltaTime;

	// Store the updated velocity back in the object
	objectToUpdate->SetActorVelocity({ updatedVelocity.x, -updatedVelocity.y, updatedVelocity.z });

	std::cout << "Updated velocity with friction: " << updatedVelocity.x << ", " << updatedVelocity.y << ", " << updatedVelocity.z << "\n";
}

glm::vec3 Scene::CalculateReflection(const glm::vec3& velocity, const glm::vec3& normal)
{
	glm::vec3 normalizedNormal = glm::normalize(normal);

	glm::vec3 reflection = velocity - 2.0f * glm::dot(velocity, normalizedNormal) * normalizedNormal;

	return reflection;
}

void Scene::SpawnObjects()
{
	float spawnPositionX = 0;
	float spawnPositionZ = 0;
	std::cout << "Enter a position between (" << minTerrainLimit.x << ", " << minTerrainLimit.z << ") and (" << maxTerrainLimit.x << ", " << maxTerrainLimit.z << ") \n";
	std::cout << "X position: "; std::cin >> spawnPositionX;
	std::cout << "Z position: "; std::cin >> spawnPositionZ;
	if (spawnPositionX < minTerrainLimit.x || spawnPositionX > maxTerrainLimit.x
		|| spawnPositionZ < minTerrainLimit.z || spawnPositionZ > maxTerrainLimit.z)
	{
		std::cout << "Error, coordinates is out of bounds! \n";
	}
	else
	{
		std::cout << "Spawned object at position (" << spawnPositionX << ", " << spawnPositionZ << ") \n";
		SpawnSetup(spawnPositionX, spawnPositionZ);
	}
}

void Scene::SpawnSetup(float spawnPositionX, float spawnPositionZ)
{
	mSceneMeshes["BSplineMesh" + std::to_string(objectsSpawned)] = std::make_shared<Mesh>(MeshShape::BSPLINE, mShader);
	mSceneBallActors["Object" + std::to_string(objectsSpawned)] = (std::make_shared<Actor>("SphereMesh", mSceneMeshes["SphereMesh"], glm::vec3{ spawnPositionX, 130.f, spawnPositionZ }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::DYNAMICOBJECT, mShader, false, ""));
	mBSplineActors["Spline" + std::to_string(objectsSpawned)] = (std::make_shared<Actor>("BSplineMesh" + std::to_string(objectsSpawned), mSceneMeshes["BSplineMesh" + std::to_string(objectsSpawned)], glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::SPLINE, mShader, false, ""));
	mBSplineActors["Spline" + std::to_string(objectsSpawned)]->ballPtr = mSceneBallActors["Object" + std::to_string(objectsSpawned)];
	objectsSpawned++;
}

void Scene::DeleteObjects()
{
	if (!mSceneBallActors.empty())
	{
		mSceneBallActors.clear();
	}
	if (!mBSplineActors.empty())
	{
		mBSplineActors.clear();
	}
}

void Scene::DrawBSplineCurve(std::shared_ptr<Actor>& objectToUpdate)
{
	if (!timerEnabled) return;
	std::string objectName = objectToUpdate->mName;
	auto& mesh = objectToUpdate->mMeshInfo;
	mesh->GenerateBSplineCurve(ballPositions[objectToUpdate->ballPtr]);
}

void Scene::ObjectPhysics(std::shared_ptr<Actor>& objectToUpdate, float deltaTime, glm::vec3& normal)
{
	// Calculate the acceleration and velocity for the actor based on the normal
	VelocityUpdate(objectToUpdate, CalculateAccelerationVector(normal), deltaTime);

	// Update the position of the actor based on the updated velocity
	glm::vec3 position = objectToUpdate->GetActorPosition();
	glm::vec3 velocity = objectToUpdate->GetActorVelocity();
	glm::vec3 positionChange = velocity * deltaTime;
	glm::vec3 newPosition = position + positionChange;

	objectToUpdate->SetActorPosition({ newPosition.x, position.y, newPosition.z });

	// Only add the position if the ball is moving and there is a significant change in x or z
	if (glm::length(velocity) > 0.01f)
	{
		if (timerEnabled)
		{
			ballPositions[objectToUpdate].emplace_back(newPosition.x, position.y, newPosition.z);
			//std::cout << "Updated position: " << newPosition.x << ", " << position.y << ", " << newPosition.z << "\n";
		}
	}

	// Updating the velocity based on friction if object is within custom area bounds
	if (newPosition.x < CustomArea[0].maxBounds.x && newPosition.x > CustomArea[0].minBounds.x &&
		newPosition.z < CustomArea[0].maxBounds.z && newPosition.z > CustomArea[0].minBounds.z)
	{
		FrictionUpdate(objectToUpdate, deltaTime, normal);
	}

	//std::cout << "Updated position: " << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << "\n";
}

glm::vec3 Scene::CalculateAccelerationVector(glm::vec3& normal)
{
	// Normalize the normal vector
	glm::vec3 normalizedNormal = glm::normalize(normal);

	// Define the gravitational constant
	const float g = 0.981f;

	// Calculate the acceleration vector using the given formula
	glm::vec3 gravity(0.0f, g * 2, 0.0f);
	glm::vec3 accelerationVector = glm::dot(gravity, normalizedNormal) * normalizedNormal;

	return accelerationVector;
}

void Scene::VelocityUpdate(std::shared_ptr<Actor>& objectToUpdate, const glm::vec3& acceleration, float deltaTime)
{
	// Getting the current velocity
	glm::vec3 currentVelocity = objectToUpdate->GetActorVelocity();

	// Updating the velocity vector
	glm::vec3 updatedVelocity = currentVelocity + deltaTime * acceleration;

	// Store the updated velocity back in the object
	objectToUpdate->SetActorVelocity(updatedVelocity);

	//std::cout << "Updated velocity: " << updatedVelocity.x << ", " << updatedVelocity.y << ", " << updatedVelocity.z << "\n";
}