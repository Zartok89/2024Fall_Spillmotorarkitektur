#include "Mesh.h"
#include <glad/glad.h>

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

	default:
		throw std::invalid_argument("Unknown mesh shape");
	}

	MeshSetup();
}

void Mesh::RenderMesh()
{
	mMeshShader->use();

	// If the mesh is a line or a point, it will only use the vertices array, else draw with indices
	glBindVertexArray(mVAO);
	if (mMeshShape == MeshShape::LINE || mMeshShape == MeshShape::LINECURVE)
	{
		glLineWidth(8.f);
		glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
		glLineWidth(5.f);
		glDrawArrays(GL_POINTS, 0, mVertices.size());
	}
	else
	{
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
		std::cout << "X: " << x << ", Y: " << pointValue << std::endl;
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
		std::cout << "X: " << x << ", Y: " << pointValue << std::endl;
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

//std::pair<glm::vec3, glm::vec3> Mesh::CalculateBoxExtent(std::vector<Vertex>& BoxVertVector)
//{
//	if (BoxVertVector.empty()) { return std::pair<glm::vec3, glm::vec3>(); }
//
//	glm::vec3 minExtent = BoxVertVector[0].mPosition;
//	glm::vec3 maxExtent = BoxVertVector[0].mPosition;
//
//	for (const auto& vertex : BoxVertVector)
//	{
//		minExtent = glm::min(minExtent, vertex.mPosition);
//		maxExtent = glm::max(maxExtent, vertex.mPosition);
//	}
//	std::pair<glm::vec3, glm::vec3> extentPair = std::make_pair(minExtent, maxExtent);
//	return extentPair;
//}