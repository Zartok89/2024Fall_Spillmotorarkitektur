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
	BSPLINEBASIS,
	BSPLINEBIQUADRIC,
	PUNKTSKY
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

	/*
	 * BSplines
	 */
	void CreateBSplineSurface(int uResolution, int vResolution, int degreeU, int degreeV, const std::vector<float>& uKnot, const std::vector<float>& vKnot, const std::vector<std::vector<glm::vec3>>& controlPoints, const std::string& customName);
	void GenerateIndicesForBSplines(int uResolution, int vResolution);
	glm::vec3 EvaluateBSplineSurface(float u, float v, int degreeU, int degreeV, const std::vector<float>& uKnot, const std::vector<float>& vKnot, const std::vector<std::vector<glm::vec3>>& controlPoints);
	glm::vec3 EvaluateBSplineNormal(float u, float v, int degreeU, int degreeV, double invUResolution, double invVResolution, const std::vector<float>& uKnot, const std::vector<float>& vKnot, const std::vector<std::vector<glm::vec3>>& controlPoints);

	/*
	 * Point Cloud Generation and Triangulation
	 */
	void CreateMeshFromPointCloud(int resolution, bool usingBSpling);
	void GenerateAndPopulateGrid(int resolution, std::vector<Vertex>& tempVertices, float minVertX, float maxVertX, float minVertZ, float maxVertZ);
	void TriangulateGrid(int gridWidth, int gridHeight, std::vector<unsigned int>& indices) ;
	void CalculateNormals();
	void GenerateSplineSurface(int resolution, const std::vector<std::vector<Vertex>>& controlPoints);

	/*
	* Mesh Utility
	*/
	std::pair<glm::vec3, glm::vec3> CalculateBoxExtent();
	float CalculateRadius();
	//std::pair<glm::vec3, glm::vec3> CalculateBoxExtent(std::vector<Vertex>& BoxVertVector);

	/*
	 * Member Variables
	 */
	VAO mVAO{ 0 };
	VBO mVBO{ 0 };
	EBO mEBO{ 0 };
	// Mesh information
	std::vector<Vertex> mVertices{};
	std::vector<Index> mIndices{};
	MeshShape mMeshShape;
	Shader* mMeshShader;
	// Terrain settings
	float mTerrainWidth{ 80.f };
	float mTerrainDepth{ 40.f };
	float mTerrainDivisionsWidth{ 40.f };
	float mTerrainDivisionsDepth{ 20.f };
	bool setWireframe{ false };

	/* BiQuadratic Spline Variables*/
	// Degrees
	int Du = 2;
	int Dv = 2;
	float B0(float t) { return 0.5f * (1 - t) * (1 - t); }
	float B1(float t) { return 0.5f + t * (1 - t); }
	float B2(float t) { return 0.5f * t * t; }

	// Resolution
	int UResolution = 20;
	int VResolution = 20;

	// Knot Vectors
	std::vector<float> uKnot = { 0.0,0.0,0.0,1.0,2.0,2.0,2.0 };
	std::vector<float> vKnot = { 0.0,0.0,0.0,1.0,2.0,2.0,2.0 };

	// Control Points
	std::vector<std::vector<glm::vec3>> controlPoints = {
	{glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,1.0f,0.0f), glm::vec3(2.0f,0.0f,0.0f)},
	{glm::vec3(0.0f,1.0f,1.0f), glm::vec3(1.0f,2.0f,1.0f), glm::vec3(2.0f,1.0f,1.0f)},
	{glm::vec3(0.0f,0.0f,2.0f), glm::vec3(1.0f,1.0f,2.0f), glm::vec3(2.0f,0.0f,2.0f)}
	};

	// Light properties
	glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };
	glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f }; // White light

	// Material properties
	glm::vec3 objectColor{ 1.0f, 0.0f, 0.2f }; // Example object color
	float ambientStrength = 1.f;
	float specularStrength = 0.5f;
	float shininess = 32.0f;
};
