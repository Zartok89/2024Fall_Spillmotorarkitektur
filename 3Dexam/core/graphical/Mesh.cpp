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

	case MeshShape::BSPLINEBASIS:
		BSplineSurface();
		break;

	case MeshShape::BSPLINEBIQUADRIC:
		BSplineSurface();
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
	if (mMeshShape == MeshShape::LINE || mMeshShape == MeshShape::LINECURVE /*|| mMeshShape == MeshShape::BSPLINEBASIS || mMeshShape == MeshShape::BSPLINEBIQUADRIC*/)
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

float Mesh::bSplineBasis(int i, float t, int degree)
{
	const std::vector<float> knots = { 0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,2.0f,2.0f,2.0f,3.0f,3.0f,3.0f };

	if (degree == 0) {
		if (knots[i] <= t && t < knots[i + 1]) {
			return 1.0f;
		}
		else {
			return 0.0f;
		}
	}
	else {
		float coeff1 = 0.0f;
		float coeff2 = 0.0f;

		// Calculate the first coefficient
		if (knots[i + degree] != knots[i]) {
			coeff1 = (t - knots[i]) / (knots[i + degree] - knots[i]);
		}

		// Calculate the second coefficient
		if (knots[i + degree + 1] != knots[i + 1])
		{
			coeff2 = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]);
		}

		// Recursively calculate the basis functions return coeff1 * bSplineBasis(i, t, degree -1) + coeff2 * bSplineBasis(i +1, t, degree -1);
	}

	//const std::vector<float> knots = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f };

	//if (degree == 0) {
	//	if (knots[i] <= t && t < knots[i + 1]) {
	//		return 1.0f;
	//	}
	//	else {
	//		return 0.0f;
	//	}
	//}
	//else {
	//	float coeff1 = 0.0f;
	//	float coeff2 = 0.0f;

	//	// Calculate the first coefficient
	//	if (knots[i + degree] != knots[i]) {
	//		coeff1 = (t - knots[i]) / (knots[i + degree] - knots[i]);
	//	}

	//	// Calculate the second coefficient
	//	if (knots[i + degree + 1] != knots[i + 1]) {
	//		coeff2 = (knots[i + degree + 1] - t) / (knots[i + degree + 1] - knots[i + 1]);
	//	}

	//	// Recursively calculate the basis functions
	//	return coeff1 * bSplineBasis(i, t, degree - 1) + coeff2 * bSplineBasis(i + 1, t, degree - 1);
	//}
}

float Mesh::bSplineBasisBiQuadratic(int i, int j, float u, float v)
{
	const int degree = 2;
	return bSplineBasis(i, u, degree) * bSplineBasis(j, v, degree);
}

void Mesh::DrawBSplineBasisFunction()
{
	//std::vector<glm::vec3> controlPoints = {
	//{0.0f,0.0f,0.0f}, // P00
	//{1.0f,0.0f,0.0f}, // P01
	//{2.0f,0.0f,0.0f}, // P02
	//{0.0f,1.0f,1.0f}, // P10
	//{1.0f,1.0f,2.0f}, // P11
	//{2.0f,1.0f,1.0f}, // P12
	//{0.0f,2.0f,0.0f}, // P20
	//{1.0f,2.0f,0.0f}, // P21
	//{2.0f,2.0f,0.0f} // P22
	//};

	std::vector<glm::vec3> controlPoints = {
 {0.0f,0.0f,0.0f}, // P00
		{1.0f,2.0f,0.0f}, // P01
		{2.0f,0.0f,0.0f}, // P02
		{0.0f,1.0f,1.0f}, // P10
		{1.0f,3.0f,2.0f}, // P11
		{2.0f,1.0f,1.0f}, // P12
		{0.0f,0.0f,0.0f}, // P20
		{1.0f,1.0f,0.0f}, // P21
		{2.0f,0.0f,0.0f} // P22};
	};

	mVertices.clear();

	// Draw B-Spline Basis Functions

	if (MeshShape::BSPLINEBASIS)
	{
		for (float t = 0.0f; t <= 3.0f; t += 0.01f) // Adjust range based on knots
		{
			Vertex point = { 0.0f, 0.0f, 0.0f }; // Initialize point as a Vertex type // Sum the contributions from all control points
			for (size_t i = 0; i < controlPoints.size(); ++i)
			{
				float basisValue = bSplineBasis(i, t, 2); // Degree2
				point.mPosition.x += controlPoints[i].x * basisValue; // Weighted x position
				point.mPosition.y += controlPoints[i].y * basisValue; // Weighted y position
				point.mPosition.z += controlPoints[i].z * basisValue; // Weighted z position
			}

			mVertices.emplace_back(point); // Add point to the vertex list
		}

		//for (int i = 0; i < 3; ++i)
		//{
		//	for (float t = 0.0f; t <= 1.0f; t += 0.01f)
		//	{
		//		float value = bSplineBasis(i, t, 2);
		//		float xPos = controlPoints[i].x * value; // X position
		//		float zPos = controlPoints[i].z; // Z position should be constant for this basis function
		//		mVertices.emplace_back(xPos, 0.0f, zPos); // Create vertex
		//	}
		//}
	}

	if (MeshShape::BSPLINEBIQUADRIC)
	{
		for (int i = 0; i < 3; ++i) // For basis functions in u direction
		{
			for (int j = 0; j < 3; ++j) // For basis functions in v direction
			{
				for (float u = 0.0f; u <= 1.0f; u += 0.01f)
				{
					for (float v = 0.0f; v <= 1.0f; v += 0.01f)
					{
						float value = bSplineBasisBiQuadratic(i, j, u, v); // Evaluate bi-quadratic basis function
						glm::vec3 controlPoint = controlPoints[i + j * 3]; // Get the control point
						mVertices.emplace_back(controlPoint.x * value, controlPoint.y * value, controlPoint.z * value); // Create vertex
					}
				}
			}
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

std::pair<glm::vec3, glm::vec3> Mesh::B2(float tu, float tv, int my_u, int my_v)
{
	glm::vec3 Bu, Bv;

	// Calculate the B-spline coefficients based on the B-spline basis functions.
	// This is a simplified approach; the coefficients need to be drawn from the actual basis functions.
	// Assuming you have functions or logic to get these:
	// Example computation (you need to implement the actual logic)
	//Bu = Vec3(/* ... compute based on tu and my_u ... */);
	//Bv = Vec3(/* ... compute based on tv and my_v ... */);

	return std::make_pair(Bu, Bv);
}

glm::vec3 Mesh::evaluateBiQuadratic(int my_u, int my_v, glm::vec3 bu, glm::vec3 bv)
{
	glm::vec3 result(0.0f, 0.0f, 0.0f);

	// Compute the point on the surface based on the B-spline coefficients
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			// Check if accessing a valid position in mc
			int index = my_u + i + (my_v + j) * n_u;

			if (index < mc.size()) {
				result += mc[index] * (bu[i] * bv[j]); // Control points times weights
			}
			else {
				// Handle cases where index is out of bounds
				// This situation should ideally not occur with correct configs.
				std::cerr << "Control point index out of bounds!" << std::endl;
			}
		}
	}
	return result;
}

void Mesh::makeBiQuadraticSurface()
{
	int nu = n_u - d_u + 1;
	int nv = n_v - d_v + 1;

	mVertices.clear();
	mIndices.clear();

	for (int i = 0; i < nv; i++)
	{
		for (int j = 0; j < nu; j++)
		{
			float u = j * h;
			float v = i * h;

			int my_u = findKnotInterval(mu, d_u, n_u, u);
			int my_v = findKnotInterval(mv, d_v, n_v, v);

			if (my_u == -1 || my_v == -1)
			{
				continue; // Handle out of bounds safely
			}

			auto coeff_pair = B2(u, v, my_u, my_v);
			glm::vec3 p = evaluateBiQuadratic(my_u, my_v, coeff_pair.first, coeff_pair.second);
			mVertices.push_back({ p.x, p.y, p.z });

			// Ensure safe setup for triangle indices
			if (i < nv - 1 && j < nu - 1)
			{
				mIndices.push_back(i * nu + j);
				mIndices.push_back(i * nu + (j + 1));
				mIndices.push_back((i + 1) * nu + j);

				mIndices.push_back(i * nu + (j + 1));
				mIndices.push_back((i + 1) * nu + (j + 1));
				mIndices.push_back((i + 1) * nu + j);
			}
		}
	}
}

void Mesh::BSplineSurface()
{
	mName = "test"; // Initialize knot vectors
	mu = { 0,0,0,1,2,2,2 }; // Knot vector for u
	mv = { 0,0,0,1,1,1 }; // Knot vector for v

	// Initialize control points
	for (int i = 0; i < n_u; i++)
	{
		mc.emplace_back(glm::vec3(i, 0, 0)); // First row of control points
	}
	mc.emplace_back(glm::vec3(0, 1, 0));
	mc.emplace_back(glm::vec3(1, 1, 2));
	mc.emplace_back(glm::vec3(2, 1, 2));
	mc.emplace_back(glm::vec3(3, 1, 0));

	for (int i = 0; i < n_u; i++)
	{
		mc.emplace_back(glm::vec3(i, 2, 0)); // Second row of control points
	}
	// Create the bi-quadratic surface
	makeBiQuadraticSurface();
}

int Mesh::findKnotInterval(std::vector<float> _mu, int _d_u, int _n_u, float _u)
{
	// Safety check for knot vector size
	if (_mu.size() < _n_u + 1) {
		return -1; // Return -1 if the knot vector is too small
	}

	// Check if the parameter is out of bounds
	if (_u < _mu[_d_u] || _u > _mu[_n_u]) {
		return -1; // Return -1 to indicate out of bounds
	}

	// Find the knot interval
	for (int i = _d_u; i < _n_u + 1; ++i) {
		if (_u < _mu[i]) {
			return i - 1; // Return the index of the knot interval
		}
	}

	// Return the last interval if parameter is at the end
	return _n_u - 1;
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