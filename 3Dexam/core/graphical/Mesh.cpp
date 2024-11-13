#include "Mesh.h"

#include <unordered_map>
#include <glad/glad.h>

#include "utility/MathLibrary.h"
#include "utility/ReadWriteFiles.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Mesh::Mesh(MeshShape meshShape, Shader* meshShader) : mMeshShape(meshShape), mMeshShader(meshShader)
{
	switch (mMeshShape)
	{
	case MeshShape::LINE:
		GenerateMathCurveFunctions(0.5f, -3, 4);
		break;

	case MeshShape::LINECURVE:
		GenerateMathCurveFunctions2(0.5f, -3, 4);
		break;

	case MeshShape::TRIANGLE:
		TriangleMesh();
		break;

	case MeshShape::SQUARE:
		SquareMesh();
		break;

	case MeshShape::CUBE:
		CubeMesh();
		break;

	case MeshShape::CUBECOLOR:
		CubeMeshColor();
		break;

	case MeshShape::SPHERE:
		SphereMesh(5.f, 10, 10);
		break;

	case MeshShape::TERRAIN_FLAT:
		GenerateFlatTerrain(mTerrainWidth, mTerrainDepth, mTerrainDivisionsWidth, mTerrainDivisionsDepth);
		break;

	case MeshShape::TERRAIN_CURVED:
		GenerateCurvedTerrain(mTerrainWidth, mTerrainDepth, mTerrainDivisionsWidth, mTerrainDivisionsDepth);
		break;

	case MeshShape::BSPLINEBASIS:
		CreateBSplineSurface(UResolution, VResolution, Du, Dv, uKnot, vKnot, controlPoints, "BSpline");
		break;

	case MeshShape::BSPLINEBIQUADRIC:
		CreateBSplineSurface(UResolution, VResolution, Du, Dv, uKnot, vKnot, controlPoints, "BSpline");
		break;

	case MeshShape::PUNKTSKY:
		CreateDotSky();
		break;

	default:
		throw std::invalid_argument("Unknown mesh shape");
	}

	MeshSetup();
}

void Mesh::RenderMesh()
{
	//mMeshShader->use();

	mMeshShader->setInt("texture1", 0);

	// Set static light properties
	mMeshShader->setVec3("lightPos", lightPos);
	mMeshShader->setVec3("lightColor", lightColor);
	mMeshShader->setVec3("objectColor", objectColor);
	mMeshShader->setFloat("ambientStrength", ambientStrength);
	mMeshShader->setFloat("specularStrength", specularStrength);
	mMeshShader->setFloat("shininess", shininess);

	// If the mesh is a line or a point, it will only use the vertices array, else draw with indices
	glBindVertexArray(mVAO);
	if (mMeshShape == MeshShape::LINE || mMeshShape == MeshShape::LINECURVE /*|| mMeshShape == MeshShape::BSPLINEBASIS || mMeshShape == MeshShape::BSPLINEBIQUADRIC*/)
	{
		if (setWireframe)
		{
			glLineWidth(3.f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		//glLineWidth(8.f);
		glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
		//glLineWidth(5.f);
		glDrawArrays(GL_POINTS, 0, mVertices.size());
	}
	if (mMeshShape == MeshShape::PUNKTSKY)
	{
		glLineWidth(5.f);
		glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	}
	else
	{
		if (setWireframe)
		{
			glLineWidth(3.f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Mesh::MeshSetup()
{
	// Setting up and binding the mesh
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

	if (mMeshShape != MeshShape::LINE)
	{
		glGenBuffers(1, &mEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(Index), mIndices.data(), GL_STATIC_DRAW);
	}

	Vertex::SetupAttributes();
}

void Vertex::SetupAttributes()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mPosition));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTexCoords));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mColor));
	glEnableVertexAttribArray(3);
}

void Mesh::TriangleMesh()
{
	mVertices = {
		// positions          // colors           // texture coords
		{-0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f},
		{0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f},
		{0.0f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.5f, 1.0f},
	};

	mIndices = {
		0, 1, 2
	};
}

void Mesh::SquareMesh()
{
	mVertices = {
		// positions          // colors           // texture coords
		{0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f}, // top right
		{0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f}, // bottom right
		{-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f}, // bottom left
		{-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f}  // top left
	};

	mIndices = {
		0, 1, 2, // first triangle
		0, 2, 3  // second triangle
	};
}

void Mesh::CubeMesh()
{
	mVertices = {
		// Front face
		{{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom-left
		{{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top-right
		{{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top-left
		// Back face
		{{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
		// Left face
		{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
		// Right face
		{{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
		// Top face
		{{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
		// Bottom face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}
	};

	mIndices = {
		// Front face
		0, 1, 2, 0, 2, 3,
		// Back face
		4, 5, 6, 4, 6, 7,
		// Left face
		8, 9, 10, 8, 10, 11,
		// Right face
		12, 13, 14, 12, 14, 15,
		// Top face
		16, 17, 18, 16, 18, 19,
		// Bottom face
		20, 21, 22, 20, 22, 23
	};
}

void Mesh::CubeMeshColor()
{
	glm::vec3 ColorVert1{ 0.5f, 0.5f, 0.5f };
	glm::vec3 ColorVert2{ 0.f, 1.f, 0.f };
	glm::vec3 ColorVert3{ 0.f, 0.f, 1.f };
	glm::vec3 ColorVert4{ 0.5f, 0.5f, 0.5f };

	mVertices = {
		{-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f},  // Red
		{ 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f},  // Green
		{ 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f},  // Blue
		{-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f},  // Yellow
		{-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f},  // Magenta
		{ 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f},  // Cyan
		{ 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f},  // White
		{-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f}   // Black
	};

	mIndices = {
		// Front face
		0, 1, 2, 2, 3, 0,
		// Right face
		1, 5, 6, 6, 2, 1,
		// Back face
		7, 6, 5, 5, 4, 7,
		// Left face
		4, 0, 3, 3, 7, 4,
		// Bottom face
		4, 5, 1, 1, 0, 4,
		// Top face
		3, 2, 6, 6, 7, 3
	};
}

void Mesh::SphereMesh(float radius, int sectorCount, int stackCount)
{
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = M_PI / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			s = (float)j / sectorCount;
			t = (float)i / stackCount;

			mVertices.emplace_back(Vertex({ x, y, z }, { nx, ny, nz }, { s, t }));
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; ++i) {
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			if (i != 0) {
				mIndices.emplace_back(k1);
				mIndices.emplace_back(k2);
				mIndices.emplace_back(k1 + 1);
			}

			if (i != (stackCount - 1)) {
				mIndices.emplace_back(k1 + 1);
				mIndices.emplace_back(k2);
				mIndices.emplace_back(k2 + 1);
			}
		}
	}
}

void Mesh::GenerateMathCurveFunctions(float resolution, float startX, float endX)
{
	// ax^2+bx+c
	float a = 2.0f;
	float b = 5.0f;
	float c = 1.f;

	for (float x = startX; x <= endX; x += resolution)
	{
		float pointValue = pow(x, a) - b * x + c;
		//std::cout << "X: " << x << ", Y: " << pointValue << std::endl;
		mVertices.emplace_back(x, 1.0f, pointValue);
	}
}
void Mesh::GenerateMathCurveFunctions2(float resolution, float startX, float endX)
{
	// ax^2+bx+c
	float a = 2.0f;
	float b = 1.0f;
	float c = 1.f;

	for (float x = startX; x <= endX; x += resolution)
	{
		float pointValue = pow(x, a) - b * x + c;
		//std::cout << "X: " << x << ", Y: " << pointValue << std::endl;
		mVertices.emplace_back(x, 1.0f, pointValue);
	}
	for (float a = 0; a <= mVertices.size() / 3; a++)
	{
		mIndices.emplace_back(a);
	}
}

void Mesh::GenerateFlatTerrain(float terrainWidth, float terrainDepth, int divisionsWidth, int divisionsDepth)
{
	int vertexCountWidth = divisionsWidth + 1;
	int vertexCountDepth = divisionsDepth + 1;

	float midWidth = terrainWidth / 2.0f;
	float midDepth = terrainDepth / 2.0f;
	float deltaX = terrainWidth / divisionsWidth;
	float deltaZ = terrainDepth / divisionsDepth;

	// Constructing the grid vertices
	for (int depthIndex = 0; depthIndex < vertexCountDepth; ++depthIndex)
	{
		float zCoord = midDepth - depthIndex * deltaZ;
		for (int widthIndex = 0; widthIndex < vertexCountWidth; ++widthIndex)
		{
			float xCoord = -midWidth + widthIndex * deltaX;
			mVertices.emplace_back(xCoord, 0.0f, zCoord, 0.0f, 0.5f, 0.1f, xCoord, zCoord);
		}
	}

	// Creating indices for rendering grid cells as two triangles
	for (int depth = 0; depth < divisionsDepth; ++depth)
	{
		for (int width = 0; width < divisionsWidth; ++width)
		{
			unsigned int currentRowStart = depth * vertexCountWidth;
			unsigned int nextRowStart = (depth + 1) * vertexCountWidth;

			// First triangle of the grid cell
			mIndices.emplace_back(currentRowStart + width);
			mIndices.emplace_back(nextRowStart + width);
			mIndices.emplace_back(currentRowStart + width + 1);

			// Second triangle of the grid cell
			mIndices.emplace_back(currentRowStart + width + 1);
			mIndices.emplace_back(nextRowStart + width);
			mIndices.emplace_back(nextRowStart + width + 1);
		}
	}
}
void Mesh::GenerateCurvedTerrain(float planeWidth, float planeDepth, int divisionsWidth, int divisionsDepth) {
	int vertexCountWidth = divisionsWidth + 1;
	int vertexCountDepth = divisionsDepth + 1;

	float midWidth = planeWidth / 2.0f;
	float midDepth = planeDepth / 2.0f;
	float deltaX = planeWidth / divisionsWidth;
	float deltaZ = planeDepth / divisionsDepth;

	// Constructing the grid vertices
	for (int depthIndex = 0; depthIndex < vertexCountDepth; ++depthIndex)
	{
		float zCoord = midDepth - depthIndex * deltaZ;
		for (int widthIndex = 0; widthIndex < vertexCountWidth; ++widthIndex)
		{
			float xCoord = -midWidth + widthIndex * deltaX;

			// Calculate yCoord based on the given mathematical function
			float yCoord = sin(xCoord) * cos(zCoord) * ((xCoord + zCoord) / 10.0f);
			mVertices.push_back(Vertex(xCoord, yCoord, zCoord, 1.0f, 1.0f, 1.0f, xCoord, zCoord));
		}
	}

	// Creating indices for rendering grid cells as two triangles
	for (int depth = 0; depth < divisionsDepth; ++depth)
	{
		for (int width = 0; width < divisionsWidth; ++width)
		{
			unsigned int currentRowStart = depth * vertexCountWidth;
			unsigned int nextRowStart = (depth + 1) * vertexCountWidth;

			// First triangle of the grid cell
			mIndices.push_back(currentRowStart + width);
			mIndices.push_back(nextRowStart + width);
			mIndices.push_back(currentRowStart + width + 1);

			// Second triangle of the grid cell
			mIndices.push_back(currentRowStart + width + 1);
			mIndices.push_back(nextRowStart + width);
			mIndices.push_back(nextRowStart + width + 1);
		}
	}
}

void Mesh::CreateBSplineSurface(int uResolution, int vResolution, int degreeU, int degreeV, const std::vector<float>& uKnot, const std::vector<float>& vKnot, const std::vector<std::vector<glm::vec3>>& controlPoints, const std::string& customName)
{
	std::string surfaceKey = customName.empty() ? "DefaultSurface" : customName;
	double invUResolution = 1.0 / (uResolution - 1);
	double invVResolution = 1.0 / (vResolution - 1);

	for (int i = 0; i < uResolution; ++i)
	{
		double u = i * invUResolution;
		for (int j = 0; j < vResolution; ++j)
		{
			double v = j * invVResolution;
			glm::vec3 surfacePoint = EvaluateBSplineSurface(u, v, degreeU, degreeV, uKnot, vKnot, controlPoints);
			glm::vec3 surfaceNormal = EvaluateBSplineNormal(u, v, degreeU, degreeV, invUResolution, invVResolution, uKnot, vKnot, controlPoints);
			glm::vec2 texCoords(static_cast<float>(u), static_cast<float>(v));
			mVertices.emplace_back(surfacePoint, surfaceNormal, texCoords);
		}
	}

	GenerateIndices(uResolution, vResolution);
}

void Mesh::GenerateIndices(int uResolution, int vResolution) {
	for (int i = 0; i < uResolution - 1; ++i) {
		for (int j = 0; j < vResolution - 1; ++j) {
			unsigned int topLeft = i * vResolution + j;
			unsigned int topRight = topLeft + 1;
			unsigned int bottomLeft = (i + 1) * vResolution + j;
			unsigned int bottomRight = bottomLeft + 1;
			mIndices.insert(mIndices.end(), { topLeft, bottomLeft, topRight, topRight, bottomLeft, bottomRight });
		}
	}
}

glm::vec3 Mesh::EvaluateBSplineSurface(float u, float v, int degreeU, int degreeV, const std::vector<float>& uKnot, const std::vector<float>& vKnot, const std::vector<std::vector<glm::vec3>>& controlPoints)
{
	glm::vec3 surfacePoint(0.0f);
	int numControlPointsU = controlPoints.size() - 1;
	int numControlPointsV = controlPoints[0].size() - 1;

	for (int i = 0; i <= numControlPointsU; ++i) {
		float uBasis = deBoorsAlgorithm::CoxDeBoorRecursive(i, degreeU, u, uKnot);
		for (int j = 0; j <= numControlPointsV; ++j) {
			float vBasis = deBoorsAlgorithm::CoxDeBoorRecursive(j, degreeV, v, vKnot);
			surfacePoint += uBasis * vBasis * controlPoints[i][j];
		}
	}

	return surfacePoint;
}

glm::vec3 Mesh::EvaluateBSplineNormal(float u, float v, int degreeU, int degreeV, double invUResolution, double invVResolution, const std::vector<float>& uKnot, const std::vector<float>& vKnot, const std::vector<std::vector<glm::vec3>>& controlPoints)
{
	float deltaU = static_cast<float>(invUResolution);
	float deltaV = static_cast<float>(invVResolution);

	glm::vec3 P = EvaluateBSplineSurface(u, v, degreeU, degreeV, uKnot, vKnot, controlPoints);
	glm::vec3 P_u = EvaluateBSplineSurface(u + deltaU, v, degreeU, degreeV, uKnot, vKnot, controlPoints);
	glm::vec3 P_v = EvaluateBSplineSurface(u, v + deltaV, degreeU, degreeV, uKnot, vKnot, controlPoints);

	glm::vec3 T_u = P_u - P;
	glm::vec3 T_v = P_v - P;

	return glm::normalize(glm::cross(T_u, T_v));
}

void Mesh::CreateDotSky()
{
	std::vector<Vertex> tempVertices;
	ReadWriteFiles::FromDataToVertexVector("TerrainData.txt", tempVertices, true);

	ReduceGridSize(tempVertices, 0.5f);
	TriangulateVector(tempVertices);
	glm::vec3 minVert = tempVertices[0].mPosition;
	glm::vec3 maxVert = tempVertices[0].mPosition;

	for (const auto& vertex : tempVertices)
	{
		minVert = glm::min(minVert, vertex.mPosition);
		maxVert = glm::max(maxVert, vertex.mPosition);
	}

	glm::vec3 midPoint = (minVert + maxVert) / glm::vec3{ 2.f };

	for (auto& vertex : tempVertices)
	{
		vertex.mPosition -= midPoint;
		mVertices.emplace_back(vertex.mPosition.x, vertex.mPosition.y, vertex.mPosition.z, vertex.mColor.r, vertex.mColor.g, vertex.mColor.b);
	}
}

void Mesh::ReduceGridSize(std::vector<Vertex>& tempVertices, float gridResolution)
{
	// Define the grid using a hash map to store grid cells
	std::unordered_map<std::string, GridCell> grid;

	// Assign points to grid cells
	for (const auto& vertex : tempVertices)
	{
		int gridX = static_cast<int>(std::floor(vertex.mPosition.x / gridResolution));
		int gridY = static_cast<int>(std::floor(vertex.mPosition.y / gridResolution));
		int gridZ = static_cast<int>(std::floor(vertex.mPosition.z / gridResolution));

		// Create a unique key for each grid cell
		std::string key = std::to_string(gridX) + "_" + std::to_string(gridY) + "_" + std::to_string(gridZ);

		// Accumulate positions and colors in the grid cell
		grid[key].mSumPosition += vertex.mPosition;
		grid[key].mSumColor += vertex.mColor;
		grid[key].mCount += 1;
	}

	// Calculate average positions and colors for each grid cell
	std::vector<Vertex> reducedVertices;
	for (const auto& [key, cell] : grid)
	{
		if (cell.mCount > 0)
		{
			Vertex avgVertex{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
			avgVertex.mPosition = cell.mSumPosition / static_cast<float>(cell.mCount);
			avgVertex.mColor = cell.mSumColor / static_cast<float>(cell.mCount);
			reducedVertices.push_back(avgVertex);
		}
	}

	// Replace tempVertices with the reduced set
	tempVertices = std::move(reducedVertices);
}

void Mesh::TriangulateVector(std::vector<Vertex>& tempVertices)
{
    size_t numVertices = tempVertices.size();  
    int gridWidth = static_cast<int>(std::sqrt(numVertices));  
    int gridHeight = gridWidth;  

    if (gridWidth * gridHeight != numVertices) 
	{  
        std::cerr << "The number of vertices does not form a perfect square grid." << std::endl;  
        return;  
    }  

    for (int i = 0; i < gridWidth - 1; ++i) 
	{  
        for (int j = 0; j < gridHeight - 1; ++j) 
		{  
            int index0 = i * gridHeight + j;  
            int index1 = (i + 1) * gridHeight + j;  
            int index2 = i * gridHeight + (j + 1);  
            int index3 = (i + 1) * gridHeight + (j + 1);  

            mIndices.push_back(index0);  
            mIndices.push_back(index1);  
            mIndices.push_back(index2);  

            mIndices.push_back(index1);  
            mIndices.push_back(index3);  
            mIndices.push_back(index2);  
        }  
    }  

	//// Finding min and max for x and y in the vector.
	//float minXVert = tempVertices[0].mPosition.x;
	//float maxXVert = tempVertices[0].mPosition.x;
	//float minYVert = tempVertices[0].mPosition.y;
	//float maxYVert = tempVertices[0].mPosition.y;
	//float minZVert = tempVertices[0].mPosition.z;
	//float maxZVert = tempVertices[0].mPosition.z;

	//for (const auto& vertex : tempVertices)
	//{
	//	minXVert = glm::min(minXVert, vertex.mPosition.x);
	//	maxXVert = glm::max(maxXVert, vertex.mPosition.x);
	//	minYVert = glm::min(minYVert, vertex.mPosition.y);
	//	maxYVert = glm::max(maxYVert, vertex.mPosition.y);
	//	minZVert = glm::min(minZVert, vertex.mPosition.z);
	//	maxZVert = glm::max(maxZVert, vertex.mPosition.z);
	//}

	//// Defining grid resolution
	//int gridWidth = static_cast<int>(std::ceil((maxXVert - minXVert) / gridResolution));
	//int gridHeight = static_cast<int>(std::ceil((maxYVert - minYVert) / gridResolution));
	//int gridDepth = static_cast<int>(std::ceil((maxZVert - minZVert) / gridResolution));

	//// Initializing the grid
	//std::vector<std::vector<std::vector<GridCell>>> grid(gridWidth, std::vector<std::vector<GridCell>>(gridHeight, std::vector<GridCell>(gridDepth)));
	//std::cout << "Grid Width: " << gridWidth << ", Grid Height: " << gridHeight << ", Grid Depth: " << gridDepth << "\n";

	//// Assigning points to the grid cells
	//for (const auto& vertex : tempVertices) {
	//	int gridX = static_cast<int>((vertex.mPosition.x - minXVert) / gridResolution);
	//	int gridY = static_cast<int>((vertex.mPosition.y - minYVert) / gridResolution);
	//	int gridZ = static_cast<int>((vertex.mPosition.z - minZVert) / gridResolution);

	//	grid[gridX][gridY][gridZ].mSumHeight += vertex.mPosition.z;
	//	grid[gridX][gridY][gridZ].mCount += 1;
	//}

	//// Calculating average heights
	//for (int i = 0; i < gridWidth; ++i)
	//{
	//	for (int j = 0; j < gridHeight; ++j)
	//	{
	//		for (int k = 0; k < gridDepth; ++k)
	//		{
	//			if (grid[i][j][k].mCount > 0)
	//			{
	//				grid[i][j][k].mSumHeight /= grid[i][j][k].mCount; // Average height
	//			}
	//		}
	//	}
	//}

	//float midXPoint = (minXVert + maxXVert) / 2.f;
	//float midYPoint = (minYVert + maxYVert) / 2.f;
	//float midZPoint = (minZVert + maxZVert) / 2.f;

	//for (auto& vertex : tempVertices)
	//{
	//	vertex.mPosition.x -= midXPoint;
	//	vertex.mPosition.y -= midYPoint;
	//	vertex.mPosition.z -= midZPoint;
	//	mVertices.emplace_back(vertex.mPosition.x, vertex.mPosition.y, vertex.mPosition.z, vertex.mColor.r, vertex.mColor.g, vertex.mColor.b);
	//}
}

std::pair<glm::vec3, glm::vec3> Mesh::CalculateBoxExtent()
{
	if (mVertices.empty()) { return std::pair<glm::vec3, glm::vec3>(); }

	glm::vec3 minExtent = mVertices[0].mPosition;
	glm::vec3 maxExtent = mVertices[0].mPosition;

	for (const auto& vertex : mVertices)
	{
		minExtent = glm::min(minExtent, vertex.mPosition);
		maxExtent = glm::max(maxExtent, vertex.mPosition);
	}
	std::pair<glm::vec3, glm::vec3> extentPair = std::make_pair(minExtent, maxExtent);
	return extentPair;
}

float Mesh::CalculateRadius()
{
	if (mVertices.empty()) { return 1.f; }

	glm::vec3 minExtent = mVertices[0].mPosition;
	glm::vec3 maxExtent = mVertices[0].mPosition;

	for (const auto& vertex : mVertices)
	{
		minExtent = glm::min(minExtent, vertex.mPosition);
		maxExtent = glm::max(maxExtent, vertex.mPosition);
	}

	float radius = (std::abs(maxExtent.x - minExtent.x) / 2);

	return radius;
}