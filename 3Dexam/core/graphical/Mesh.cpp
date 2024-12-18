#include "Mesh.h"

#include <algorithm>
#include <numeric>
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
		SphereMesh(1.f, 20, 20);
		break;

	case MeshShape::PUNKTSKY:
		CreateMeshFromPointCloud(150, false, { 1.f, 1.f, 1.f });
		break;

	case MeshShape::BSPLINE:
		GeneratePlaceholderBSplineCurve();
		break;

	default:
		throw std::invalid_argument("Unknown mesh shape");
	}

	MeshSetup();
}

void Mesh::RenderMesh()
{
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
	if (mMeshShape == MeshShape::LINE || mMeshShape == MeshShape::LINECURVE || mMeshShape == MeshShape::BSPLINE)
	{
		glLineWidth(3.f);
		glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
		glPointSize(2.f);
		glDrawArrays(GL_POINTS, 0, mVertices.size());
	}
	//if (mMeshShape == MeshShape::PUNKTSKY)
	//{
	//	glPointSize(7.f);
	//	glDrawArrays(GL_POINTS, 0, mVertices.size());
	//}
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

	if (mMeshShape != MeshShape::BSPLINE)
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

			mVertices.emplace_back(Vertex({ x, y, z }, { 1.f, 0.f, 0.f }, { nx, ny, nz }));
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

void Mesh::GeneratePlaceholderBSplineCurve()
{
	// Placeholder control points
	std::vector<glm::vec3> placeholderPoints = {};

	// Generate the B-spline curve using the placeholder points
	GenerateBSplineCurve(placeholderPoints);
}

void Mesh::GenerateBSplineCurve(std::vector<glm::vec3>& positionVector)
{
	// Amount of control points
	int numControlPoints = positionVector.size();

	// B-spline degree
	int degree = 3;

	// Amount of points on the curve
	int numCurvePoints = 100;

	// Knot vector
	std::vector<float> knots(numControlPoints + degree + 1);
	for (int i = 0; i <= degree; ++i)
	{
		knots[i] = 0.0f;
	}
	for (int i = degree + 1; i < numControlPoints; ++i)
	{
		knots[i] = static_cast<float>(i - degree) / (numControlPoints - degree);
	}
	for (int i = numControlPoints; i <= numControlPoints + degree; ++i)
	{
		knots[i] = 1.0f;
	}

	// Clear previous vertices
	mVertices.clear();

	// Generate points
	for (int i = 0; i < numCurvePoints; ++i)
	{
		float t = static_cast<float>(i) / (numCurvePoints - 1);
		glm::vec3 curvePoint(0.0f);
		for (int j = 0; j < numControlPoints; ++j)
		{
			float basis = BSplineBasis(j, degree, t, knots);
			curvePoint += basis * positionVector[j];
		}
		mVertices.emplace_back(curvePoint.x, curvePoint.y, curvePoint.z, 0.f, 1.f, 0.f);
	}

	// Update the mesh with the new vertices
	MeshSetup();
}

float Mesh::BSplineBasis(int i, int degree, float t, const std::vector<float>& knots)
{
	if (degree == 0)
	{
		return (knots[i] <= t && t < knots[i + 1]) ? 1.0f : 0.0f;
	}
	else
	{
		float left = 0.0f;
		float right = 0.0f;

		float dLeft = knots[i + degree] - knots[i];
		if (dLeft != 0.0f)
		{
			left = (t - knots[i]) / dLeft * BSplineBasis(i, degree - 1, t, knots);
		}

		float dRight = knots[i + degree + 1] - knots[i + 1];
		if (dRight != 0.0f)
		{
			right = (knots[i + degree + 1] - t) / dRight * BSplineBasis(i + 1, degree - 1, t, knots);
		}

		return left + right;
	}
}

void Mesh::CreateMeshFromPointCloud(int resolution, bool usingBSpling, glm::vec3 cloudScale)
{
	// Loading vertices from file into a temp vector
	std::vector<Vertex> tempVertices;
	ReadWriteFiles::FromDataToVertexVector("TerrainData.txt", tempVertices, true);

	// Finding min and max for x and z
	float minVertX = tempVertices[0].mPosition.x;
	float maxVertX = tempVertices[0].mPosition.x;
	float minVertZ = tempVertices[0].mPosition.z;
	float maxVertZ = tempVertices[0].mPosition.z;

	for (const auto& vertex : tempVertices)
	{
		minVertX = std::min(minVertX, vertex.mPosition.x);
		maxVertX = std::max(maxVertX, vertex.mPosition.x);
		minVertZ = std::min(minVertZ, vertex.mPosition.z);
		maxVertZ = std::max(maxVertZ, vertex.mPosition.z);
	}
	std::cout << "Min and Max found for x and z\n";

	// Calculating the midpoint and recenter vertices
	float midPointX = (minVertX + maxVertX) / 2.f;
	float midPointZ = (minVertZ + maxVertZ) / 2.f;

	for (auto& vertex : tempVertices)
	{
		vertex.mPosition.x -= midPointX;
		vertex.mPosition.z -= midPointZ;
	}

	std::cout << "Vertices recentered\n";

	if (usingBSpling)
	{
		// Calculate grid dimensions based on resolution
		int gridWidth = resolution;
		int gridHeight = resolution;

		// Calculate spacing between grid points
		float xSpacing = (maxVertX - minVertX) / (gridWidth - 1);
		float zSpacing = (maxVertZ - minVertZ) / (gridHeight - 1);

		// Initialize control points grid
		std::vector<std::vector<Vertex>> controlPoints(gridWidth, std::vector<Vertex>(gridHeight, Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)));

		// Populate the control points grid
		for (const auto& vertex : tempVertices) {
			int i = static_cast<int>((vertex.mPosition.x - minVertX) / xSpacing);
			int j = static_cast<int>((vertex.mPosition.z - minVertZ) / zSpacing);

			i = std::clamp(i, 0, gridWidth - 1);
			j = std::clamp(j, 0, gridHeight - 1);

			if (controlPoints[i][j].mPosition == glm::vec3(0.0f))
			{
				controlPoints[i][j] = vertex;
			}
			else
			{
				controlPoints[i][j].mPosition = (controlPoints[i][j].mPosition + vertex.mPosition) / 2.0f;
				controlPoints[i][j].mColor = (controlPoints[i][j].mColor + vertex.mColor) / 2.0f;
			}
		}

		// Generate the spline surface
		GenerateSplineSurface(resolution, controlPoints);
	}
	else
	{
		// Generating and Populating the grid
		GenerateAndPopulateGrid(resolution, tempVertices, minVertX, maxVertX, minVertZ, maxVertZ, cloudScale);
		std::cout << "Grid generated and populated\n";
	}

	// Triangulate the grid
	TriangulateGrid(resolution, resolution, mIndices);
	std::cout << "Grid triangulated\n";

	// Calculating normals for each triangle
	CalculateNormals();
	std::cout << "Triangulated vertices normal calculated\n";

	// Storing terrain min and max limits from the new mVertices vector
	for (const auto& vertices : mVertices)
	{
		minTerrainLimit = glm::min(minTerrainLimit, vertices.mPosition);
		maxTerrainLimit = glm::max(maxTerrainLimit, vertices.mPosition);
	}

	std::cout << "__Min and Max Terrain Limit set__\n";
	std::cout << "Min x: " << minTerrainLimit.x << "Min y: " << minTerrainLimit.y << "Min z: " << minTerrainLimit.z << "\n";
	std::cout << "Max x: " << maxTerrainLimit.x << "Max y: " << maxTerrainLimit.y << "Max z: " << maxTerrainLimit.z << "\n";
}

void Mesh::GenerateAndPopulateGrid(int resolution, std::vector<Vertex>& tempVertices, float minVertX, float maxVertX, float minVertZ, float maxVertZ, glm::vec3 cloudScale)
{
	std::cout << "Starting grid calculations with resolution: " << resolution << "\n";
	// Calculate the width and height of the grid
	float gridWidth = maxVertX - minVertX;
	float gridHeight = maxVertZ - minVertZ;

	// Determine the spacing between grid points
	float xSpacing = gridWidth / static_cast<float>(resolution - 1);
	float zSpacing = gridHeight / static_cast<float>(resolution - 1);
	float cellSize = std::min(xSpacing, zSpacing);
	float halfBox = cellSize / 2.f;

	// Calculate the number of cells in the X and Z directions
	int cellsX = static_cast<int>(std::ceil(gridWidth / cellSize));
	int cellsZ = static_cast<int>(std::ceil(gridHeight / cellSize));
	std::cout << "Grid calculations complete\n";

	// Create a 3D vector to hold vertices in each grid cell
	std::vector<std::vector<std::vector<Vertex>>> gridVector(cellsX, std::vector<std::vector<Vertex>>(cellsZ));

	// Populate the grid with vertices based on their positions
	for (const auto& vertex : tempVertices)
	{
		// Determine the cell indices for the current vertex
		int cellX = std::clamp(static_cast<int>(std::floor(vertex.mPosition.x / cellSize)), 0, cellsX - 1);
		int cellZ = std::clamp(static_cast<int>(std::floor(vertex.mPosition.z / cellSize)), 0, cellsZ - 1);
		gridVector[cellX][cellZ].emplace_back(vertex);
	}
	std::cout << "Grid populated\n";

	// Custom area for friction
	customArea.emplace_back(glm::vec3{ -40.0f, 0.0f, 0.0f }, glm::vec3{ -30.0f, 0.0f, 10.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, 0.5f);

	// Iterate over the grid to calculate average positions and colors
	for (int i = 0; i < resolution; ++i)
	{
		// Calculate the X position for the grid cell
		float posX = -gridWidth / 2.f + i * xSpacing;
		for (int j = 0; j < resolution; ++j)
		{
			// Calculate the Z position for the grid cell
			float posZ = -gridHeight / 2.f + j * zSpacing;

			// Define the bounding box for the current grid cell
			float boxMinX = posX - halfBox;
			float boxMaxX = posX + halfBox;
			float boxMinZ = posZ - halfBox;
			float boxMaxZ = posZ + halfBox;

			// Determine the range of cells that intersect with the bounding box
			int minCellX = std::clamp(static_cast<int>(std::floor(boxMinX / cellSize)), 0, cellsX - 1);
			int maxCellX = std::clamp(static_cast<int>(std::floor(boxMaxX / cellSize)), 0, cellsX - 1);
			int minCellZ = std::clamp(static_cast<int>(std::floor(boxMinZ / cellSize)), 0, cellsZ - 1);
			int maxCellZ = std::clamp(static_cast<int>(std::floor(boxMaxZ / cellSize)), 0, cellsZ - 1);

			// Vectors to hold Y values and color components
			std::vector<float> yValues, rValues, gValues, bValues;

			// Collect data from the intersecting cells
			for (int cellX = minCellX; cellX <= maxCellX; ++cellX)
			{
				for (int cellZ = minCellZ; cellZ <= maxCellZ; ++cellZ)
				{
					for (const auto& dataVertex : gridVector[cellX][cellZ])
					{
						// Check if the vertex is within the bounding box
						if (dataVertex.mPosition.x >= boxMinX && dataVertex.mPosition.x <= boxMaxX &&
							dataVertex.mPosition.z >= boxMinZ && dataVertex.mPosition.z <= boxMaxZ) {
							yValues.push_back(dataVertex.mPosition.y);
							rValues.push_back(dataVertex.mColor.r);
							gValues.push_back(dataVertex.mColor.g);
							bValues.push_back(dataVertex.mColor.b);
						}
					}
				}
			}

			// Calculate the average Y position
			float avgY;
			if (!yValues.empty())
			{
				avgY = std::accumulate(yValues.begin(), yValues.end(), 0.0f) / yValues.size();
			}
			else
			{
				avgY = 0.0f;
			}

			// Calculate the average color
			glm::vec3 avgColor;
			if (!rValues.empty())
			{
				avgColor = glm::vec3(
					std::accumulate(rValues.begin(), rValues.end(), 0.0f) / rValues.size(),
					std::accumulate(gValues.begin(), gValues.end(), 0.0f) / gValues.size(),
					std::accumulate(bValues.begin(), bValues.end(), 0.0f) / bValues.size()
				);
			}
			else
			{
				avgColor = glm::vec3(1.0f, 1.0f, 1.0f);
			}

			// Check if the current position is within the custom area
			for (const auto& area : customArea)
			{
				if (posX >= area.minBounds.x && posX <= area.maxBounds.x &&
					posZ >= area.minBounds.z && posZ <= area.maxBounds.z)
				{
					avgColor = area.color;
					//terrainFriction = area.areaFriction;
				}
			}

			// Add the averaged vertex to the mesh
			mVertices.emplace_back(posX * cloudScale.x, avgY * cloudScale.y, posZ * cloudScale.z, avgColor.r, avgColor.g, avgColor.b);
		}
		int percentageComplete = (i / static_cast<float>(resolution)) * 100;
		std::cout << "Grid generation " << percentageComplete << "% complete" << "\n";
	}
}

void Mesh::TriangulateGrid(int gridWidth, int gridHeight, std::vector<Index>& indices)
{
	indices.clear();

	for (int i = 0; i < gridWidth - 1; ++i)
	{
		for (int j = 0; j < gridHeight - 1; ++j)
		{
			// Calculate indices for the two triangles in this grid cell
			unsigned int index0 = i * gridHeight + j;
			unsigned int index1 = (i + 1) * gridHeight + j;
			unsigned int index2 = i * gridHeight + (j + 1);
			unsigned int index3 = (i + 1) * gridHeight + (j + 1);

			// First triangle (index0, index1, index2)
			indices.push_back(index0);
			indices.push_back(index1);
			indices.push_back(index2);

			// Second triangle (index1, index3, index2)
			indices.push_back(index1);
			indices.push_back(index3);
			indices.push_back(index2);
		}
	}
}

void Mesh::CalculateNormals()
{
	// Step 1: Initialize normals to zero
	for (auto& vertex : mVertices)
	{
		vertex.mNormal = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// Step 2: Iterate over each triangle
	for (size_t i = 0; i < mIndices.size(); i += 3)
	{
		unsigned int index0 = mIndices[i];
		unsigned int index1 = mIndices[i + 1];
		unsigned int index2 = mIndices[i + 2];

		glm::vec3& v0 = mVertices[index0].mPosition;
		glm::vec3& v1 = mVertices[index1].mPosition;
		glm::vec3& v2 = mVertices[index2].mPosition;

		// Calculate the edges of the triangle
		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		// Calculate the normal using the cross product
		glm::vec3 normal = glm::cross(edge1, edge2);

		// Accumulate the normal to each vertex of the triangle
		mVertices[index0].mNormal += normal;
		mVertices[index1].mNormal += normal;
		mVertices[index2].mNormal += normal;
	}

	// Step 3: Normalize the accumulated normals
	for (auto& vertex : mVertices)
	{
		vertex.mNormal = glm::normalize(vertex.mNormal);
	}
}

void Mesh::GenerateSplineSurface(int resolution, const std::vector<std::vector<Vertex>>& controlPoints)
{
	int gridWidth = controlPoints.size();
	int gridHeight = controlPoints[0].size();

	mVertices.clear();

	// Evaluate the spline surface at each grid point
	for (int i = 0; i < resolution; ++i) {
		float u = i / static_cast<float>(resolution - 1);
		for (int j = 0; j < resolution; ++j) {
			float v = j / static_cast<float>(resolution - 1);

			glm::vec3 position(0.0f);
			glm::vec3 color(0.0f);

			for (int du = 0; du < 3; ++du) {
				for (int dv = 0; dv < 3; ++dv) {
					int indexU = std::clamp(i + du - 1, 0, gridWidth - 1);
					int indexV = std::clamp(j + dv - 1, 0, gridHeight - 1);

					float basisU = (du == 0) ? B0(u) : (du == 1) ? B1(u) : B2(u);
					float basisV = (dv == 0) ? B0(v) : (dv == 1) ? B1(v) : B2(v);

					position += basisU * basisV * controlPoints[indexU][indexV].mPosition;
					color += basisU * basisV * glm::vec3(controlPoints[indexU][indexV].mColor);
				}
			}

			mVertices.emplace_back(position.x, position.y, position.z, color.r, color.g, color.b);
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