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
	bool setWireframe{false};

	// Define degrees 2 = Bi-Quadratic
	int Du = 2;
	int Dv = 2;

	// Define resolution
	int UResolution = 20;
	int VResolution = 20;

	// Define knot vectors
	std::vector<float> uKnot = { 0.0, 0.0, 0.0, 1.0, 2.0, 2.0, 2.0 };
	std::vector<float> vKnot = { 0.0, 0.0, 0.0, 1.0, 2.0, 2.0, 2.0 };

	// Define control points (2D grid)
	std::vector<std::vector<glm::vec3>> controlPoints = {
		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f)},
		{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(2.0f, 1.0f, 1.0f)},
		{glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 2.0f), glm::vec3(2.0f, 0.0f, 2.0f)}
	};

	//AddActorToLevel(BSplineSurface);

	void CreateBSplineSurface(int _UResolution, int _VResolution, int _du,
		int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot,
		const std::vector<std::vector<glm::vec3>>& _controlPoints, std::string _customName);

	glm::vec3 EvaluateBSplineSurface(float _u, float _v, int _du, int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints);

	glm::vec3 EvaluateBSplineNormal(float _u, float _v, int _du, int _dv, int _UResolution, int _VResolution, const std::vector<float>& _uKnot,
		const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints);

	float CoxDeBoorRecursive(int _i, int _d, float _uv, const std::vector<float>& _knotVector);

	///* TESTING DAG FUNCTION */ // ------NOPE-------- ///
	//int n_u = 4; // Number of control points in u direction
	//int n_v = 3; // Number of control points in v direction
	//int d_u = 2; // Degree in u direction
	//int d_v = 2; // Degree in v direction
	//float h = 0.1f; // Spacing
	//std::string mName;
	//std::vector<float> mu; // Knot vector for u
	//std::vector<float> mv; // Knot vector for v
	//std::vector<glm::vec3> mc; // Control points
	//std::pair<glm::vec3, glm::vec3> B2(float tu, float tv, int my_u, int my_v);
	//glm::vec3 evaluateBiQuadratic(int my_u, int my_v, glm::vec3 bu, glm::vec3 bv);
	//void makeBiQuadraticSurface();
	//void BSplineSurface();
	//int findKnotInterval(std::vector<float> _mu, int _d_u, int _n_u, float _u);
};
