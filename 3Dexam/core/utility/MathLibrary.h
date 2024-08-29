#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "graphical/Mesh.h"

//struct MathVertex
//{
//	std::vector<MathVertex> mMathVertices;
//
//	float x{0.f};
//	float y{0.f};
//	float z{0.f};
//};

/*
 * Math Algorithms - Chapter 4, 6.
 */
class LeastSquaresMethod
{
public:
	glm::vec3 LeastSquaresMethodAlgorithm(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
	void AddVertices(std::vector<Vertex>& verticesVector, float drawStartValue, float drawEndValue, float curveResolution, glm::vec3 functionVariables);
};

class BarycentricCoordinates
{
};

/*
 * Basic Functions - Chapter 5.
 */
class BernsteinBF
{
};

class LaGrangeBF
{
};

class BSplineBF
{
	void FindingDerivedFunction();
};

/*
 * Parametric Curves - Chapter 7.
 */

class BezierCurves
{
};

class SplineCurves
{
};

class NevillesAlgorithm
{
};

class deBoorsAlgorithm
{
};