#include "MathLibrary.h"

#include <iostream>
#include <string>
#include <glm/ext/matrix_transform.hpp>
#include <numeric>
#include <algorithm>
#include <ratio>

#include "graphical/Mesh.h"

#define PrintVec3(variable) std::cout << "x: " << variable.x << ", " << "y: " << variable.y << ", "  << "z: " << variable.z << std::endl

glm::vec3 LeastSquaresMethod::LeastSquaresMethodAlgorithm(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3)
{
	// Adding points into the ax^2 + bx + c = y

	glm::mat3 matrixA = glm::mat3x3
	{
		(point1.x * point1.x), point1.x, 1,
		(point2.x * point2.x), point2.x, 1,
		(point3.x * point3.x), point3.x, 1
	};

	glm::vec3 matrixB = glm::vec3
	{
		point1.y, point2.y,point3.y
	};

	glm::mat3 matrixAT = glm::mat3
	{
		(point1.x * point1.x), (point2.x * point2.x), (point3.x * point3.x),
		point1.x, point2.x, point3.x,
		1, 1, 1
	};

	//----- Order of operation is important! Kinda reversed multiplication in glm -----//
	// Transposed Matrix 1 * Matrix 1
	glm::mat3 ATA = matrixA * matrixAT;

	// Transposed Matrix 1 * Matrix B
	glm::vec3 b = matrixB * matrixAT;

	// Inverted ATA matrix
	glm::mat3 invertedMatrix = glm::inverse(ATA);

	// Inverted ATA matrix * b
	glm::vec3 determinants = b * invertedMatrix;

	std::cout << "MatrixA values: \n";
	for (int i = 0; i < 3; i++)
	{
		PrintVec3(matrixA[i]);
	}
	std::cout << std::endl;

	std::cout << "MatrixAT values: \n";
	for (int i = 0; i < 3; i++)
	{
		PrintVec3(matrixAT[i]);
	}
	std::cout << std::endl;

	std::cout << "Multiplied Matrix values: \n";
	for (int i = 0; i < 3; i++)
	{
		PrintVec3(ATA[i]);
	}
	std::cout << std::endl;

	std::cout << "Y values: \n";
	for (int i = 0; i < 1; i++)
	{
		PrintVec3(matrixB);
	}
	std::cout << std::endl;

	std::cout << "b values: \n";
	for (int i = 0; i < 1; i++)
	{
		PrintVec3(b);
	}
	std::cout << std::endl;

	std::cout << "Interted MatrixA values: \n";
	for (int i = 0; i < 3; i++)
	{
		PrintVec3(invertedMatrix[i]);
	}
	std::cout << std::endl;

	std::cout << "determinant values: \n";
	for (int i = 0; i < 1; i++)
	{
		PrintVec3(determinants);
	}
	std::cout << std::endl;

	std::cout << "The Least Squares Method function for points " << std::endl;
	PrintVec3(point1);
	PrintVec3(point2);
	PrintVec3(point3);
	std::cout << "is: " << determinants.x << "*x^2 + " << determinants.y << "*x + " << determinants.z << "\n";

	return determinants;
}

void LeastSquaresMethod::AddVertices(std::vector<Vertex>& verticesVector, float drawStartValue, float drawEndValue,
	float curveResolution, glm::vec3 functionVariables)
{
	// Using function to add points into vector to draw the curve
	for (float i = drawStartValue; i <= drawEndValue; i += curveResolution)
	{
		Vertex tempVec{ 0, 0, 0 };
		tempVec.mPosition.x = functionVariables.x * pow(i, 2);
		tempVec.mPosition.y = functionVariables.y * i;
		tempVec.mPosition.z = functionVariables.z;
		float yValue = tempVec.mPosition.x + tempVec.mPosition.y + tempVec.mPosition.z;
		std::cout << "x: " << i << ", y: " << yValue << "\n";
		verticesVector.emplace_back(i, yValue, 1.f, 0.f, 1.f, 0.f);
	}
}