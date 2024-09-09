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
	int AmountOfBalls = 1000;

	glm::vec3 TempVec = glm::vec3{ 0.f, 0.f, 0.f };
	for (int i = 0; i <= AmountOfBalls; i++)
	{
		mSceneActors["SphereObject " + std::to_string(i)] = (std::make_shared<Actor>("SphereMesh", mSceneMeshes["SphereMesh"], TempVec, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 0.1f, Actor::ActorType::BOUNCINGBALL, mShader, "BlueTexture"));
		TempVec = RandomNumberGenerator->GeneratorRandomVector(0, 100);
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
		BallBouncingAround(deltaTime, actor);
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

void Scene::BallBouncingAround(float deltaTime, std::shared_ptr<Actor>& actor)
{
    glm::vec3 targetPosition = RandomNumberGenerator->GeneratorRandomVector(0, 100);  
    glm::vec3 currentPosition = actor->GetActorPosition();  

    float lerpFactor = 0.3f; 

    glm::vec3 newPosition = glm::lerp(currentPosition, targetPosition, lerpFactor * deltaTime);  

    actor->SetActorPosition(newPosition);
}

// Gammel eksamen-scene
//void Scene::LoadActors()
//{
//	/*
//	 * Scene 2D shapes
//	 */
//	 //mSceneActors["Triangle"] = (std::make_shared<Actor>("TriangleMesh", glm::vec3{ 0.f, 3.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 90.f, 1.f, Actor::ActorType::STATIC, mShader, "BlueTexture"));
//	mSceneActors["Square"] = (std::make_shared<Actor>("SquareMesh", mSceneMeshes["SquareMesh"], glm::vec3{ 0.f, 0.f, -20.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 50.f, Actor::ActorType::STATIC, mShader, "SkyTexture"));
//
//	/*
//	 * Scene objects
//	 */
//	mSceneActors["NPCLineFollow"] = (std::make_shared<Actor>("CubeMeshColor", mSceneMeshes["CubeMeshColor"], glm::vec3{ 4.f, 0.f, -0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 0.5f, Actor::ActorType::NPC_FOLLOWLINE, mShader));
//	mSceneActors["NPCCurveFollow"] = (std::make_shared<Actor>("CubeMesh", mSceneMeshes["CubeMesh"], glm::vec3{ 6.f, 0.f, -0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 0.5f, Actor::ActorType::NPC_FOLLOWCURVE, mShader, "BlueTexture"));
//	mSceneActors["StaticCube"] = (std::make_shared<Actor>("CubeMesh", mSceneMeshes["CubeMesh"], glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::STATIC, mShader, "BlueTexture"));
//	mSceneActors["PlayerCube"] = (std::make_shared<Actor>("CubeMeshColor", mSceneMeshes["CubeMeshColor"], glm::vec3{ -4.f, 0.f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::PLAYER, mShader));
//	mSceneActors["NPCCube"] = (std::make_shared<Actor>("CubeMesh", mSceneMeshes["CubeMesh"], glm::vec3{ 0.f, 0.f, -3.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::NPC, mShader, "BlueTexture"));
//
//	/*
//	 * Scene terrain
//	 */
//	//mSceneActors["Terrain"] = (std::make_shared<Actor>("FlatTerrainMesh", glm::vec3{ 0.f , -0.55f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::STATIC, mShader));
//	mSceneActors["Terrain"] = (std::make_shared<Actor>("CurvedTerrainMesh", mSceneMeshes["CurvedTerrainMesh"], glm::vec3{ 0.f , -0.55f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::STATIC, mShader, "GrassTexture"));
//
//	/*
//	 * Scene math functions
//	 */
//	mSceneActors["LineTest"] = (std::make_shared<Actor>("LineMesh", mSceneMeshes["LineMesh"], glm::vec3{ -10.f, -1.f, -5.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::STATIC, mShader));
//	mSceneActors["LineCurve"] = (std::make_shared<Actor>("LineCurvedMesh", mSceneMeshes["LineCurvedMesh"], glm::vec3{ 10.f, -1.f, -5.f }, glm::vec3{ 1.f, 0.f, 0.f }, 0.f, 1.f, Actor::ActorType::CURVETOFOLLOW, mShader));
//}