#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "utility/ICollisionBounds.h"
#include "shader/Shader.h"
#include "utility/ReadWriteFiles.h"
#include "utility/VariableTypes.h"

enum MeshShape
{
	LINE,
	LINECURVE,
	TRIANGLE,
	SQUARE,
	CUBE,
	SPHERE,
	CUBECOLOR,
	TERRAIN_FLAT,
	TERRAIN_CURVED,
	PUNKTSKY,
	BSPLINE
};

class CustomArea
{
public:
	glm::vec3 minBounds{ 0.f, 0.f, 0.f };
	glm::vec3 maxBounds{ 0.f, 0.f, 0.f };
	glm::vec3 color{ 0.f, 1.f, 0.f };
	float areaFriction;

	CustomArea(glm::vec3 minimumBounds, glm::vec3 maximumBounds, glm::vec3 boundsColor, float _areaFriction)
	{
		minBounds = minimumBounds;
		maxBounds = maximumBounds;
		color = boundsColor;
		areaFriction = _areaFriction;
	}
};

class Vertex
{
public:
	glm::vec3 mPosition{ 0.0f };
	glm::vec3 mNormal{ 0.0f };
	glm::vec2 mTexCoords{ 0.0f };
	glm::vec3 mColor{ 0.0f };
	glm::vec3 mBerycentricCoordinate{ 0.0f };

	Vertex(float x, float y, float z)
	{
		mPosition = glm::vec3(x, y, z);
		mNormal = glm::vec3(1.f);
		mTexCoords = glm::vec2(1.f);
	}

	Vertex(float x, float y, float z, float r, float g, float b)
	{
		mPosition = glm::vec3(x, y, z);
		mColor = glm::vec3(r, g, b);
	}

	Vertex(float x, float y, float z, float r, float g, float b, float u, float v)
	{
		mPosition = glm::vec3(x, y, z);
		mColor = glm::vec3(r, g, b);
		mTexCoords = glm::vec2(u, v);
	}

	Vertex(float x, float y, float z, float r, float g, float b, float u, float v, float bx, float by, float bz)
	{
		mPosition = glm::vec3(x, y, z);
		mColor = glm::vec3(r, g, b);
		mTexCoords = glm::vec2(u, v);
		mBerycentricCoordinate = glm::vec3(bx, by, bz);
	}

	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoords)
	{
		mPosition = position;
		mNormal = normal;
		mTexCoords = texCoords;
	}

	Vertex(const glm::vec3& position, const glm::vec3& colors, const glm::vec3& normals)
	{
		mPosition = position;
		mColor = colors;
		mNormal = normals;
	}

	static void SetupAttributes();
};

class Mesh
{
public:
	/*
	 * Constructor, setup and render
	 */
	Mesh(MeshShape meshShape, Shader* meshShader);
	void RenderMesh();
	void MeshSetup();

	/*
	 * Mesh shapes
	 */
	void TriangleMesh();
	void SquareMesh();
	void CubeMesh();
	void CubeMeshColor();
	void SphereMesh(float radius, int sectorCount, int stackCount);

	/*
	 * Mesh generations
	 */
	void GenerateMathCurveFunctions(float resolution, float startX, float endX);
	void GenerateMathCurveFunctions2(float resolution, float startX, float endX);
	void GenerateFlatTerrain(float terrainWidth, float terrainDepth, int divisionsWidth, int divisionsDepth);
	void GenerateCurvedTerrain(float planeWidth, float planeDepth, int divisionsWidth, int divisionsDepth);
	void GeneratePlaceholderBSplineCurve();
	void GenerateBSplineCurve(std::vector<glm::vec3>& positionVector);
	float BSplineBasis(int i, int degree, float t, const std::vector<float>& knots);

	/*
	 * Point Cloud Generation and Triangulation
	 */
	void CreateMeshFromPointCloud(int resolution, bool usingBSpling, glm::vec3 cloudScale);
	void GenerateAndPopulateGrid(int resolution, std::vector<Vertex>& tempVertices, float minVertX, float maxVertX, float minVertZ, float maxVertZ, glm::vec3 cloudScale);
	void TriangulateGrid(int gridWidth, int gridHeight, std::vector<unsigned int>& indices);
	void CalculateNormals();
	void GenerateSplineSurface(int resolution, const std::vector<std::vector<Vertex>>& controlPoints);

	/*
	* Mesh Utility
	*/
	std::pair<glm::vec3, glm::vec3> CalculateBoxExtent();
	float CalculateRadius();

	/*
	 * Member Variables
	 */
	 /*Bindings*/
	VAO mVAO{ 0 };
	VBO mVBO{ 0 };
	EBO mEBO{ 0 };

	/*Mesh information*/
	std::vector<Vertex> mVertices{};
	std::vector<Index> mIndices{};
	MeshShape mMeshShape;
	Shader* mMeshShader;

	/*Terrain settings*/
	float mTerrainWidth{ 80.f };
	float mTerrainDepth{ 40.f };
	float mTerrainDivisionsWidth{ 40.f };
	float mTerrainDivisionsDepth{ 20.f };
	bool setWireframe{ false };
	glm::vec3 minTerrainLimit{ 0.f, 0.f, 0.f, };
	glm::vec3 maxTerrainLimit{ 0.f, 0.f, 0.f, };
	std::vector<CustomArea> customArea;

	/*BiQuadratic Spline Variables*/
	float B0(float t) { return 0.5f * (1 - t) * (1 - t); }
	float B1(float t) { return 0.5f + t * (1 - t); }
	float B2(float t) { return 0.5f * t * t; }

	/*Light properties*/
	glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };
	glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f }; // White light

	/*Material properties*/
	glm::vec3 objectColor{ 1.0f, 0.0f, 0.2f }; // Example object color
	float ambientStrength = 1.f;
	float specularStrength = 0.5f;
	float shininess = 32.0f;
};
