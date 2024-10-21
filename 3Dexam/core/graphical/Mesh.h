#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "utility/ICollisionBounds.h"
#include "shader/Shader.h"
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
	BSPLINEBIQUADRIC
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
	float bSplineBasis(int i, float t, int degree);
	float bSplineBasisBiQuadratic(int i, int j, float u, float v);
	void DrawBSplineBasisFunction();

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

	/* TESTING DAG FUNCTION */
	int n_u = 4; // Number of control points in u direction
	int n_v = 3; // Number of control points in v direction
	int d_u = 2; // Degree in u direction
	int d_v = 2; // Degree in v direction
	float h = 0.1f; // Spacing
	std::string mName;
	std::vector<float> mu; // Knot vector for u
	std::vector<float> mv; // Knot vector for v
	std::vector<glm::vec3> mc; // Control points
	std::pair<glm::vec3, glm::vec3> B2(float tu, float tv, int my_u, int my_v);
	glm::vec3 evaluateBiQuadratic(int my_u, int my_v, glm::vec3 bu, glm::vec3 bv);
	void makeBiQuadraticSurface();
	void BSplineSurface();
	int findKnotInterval(std::vector<float> _mu, int _d_u, int _n_u, float _u);

};
