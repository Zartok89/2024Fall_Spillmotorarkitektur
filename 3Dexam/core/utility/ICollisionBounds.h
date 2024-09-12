#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <vector>

class Vertex;

class ICollisionBounds
{
public:
	glm::vec3 mCenter{ 0.f, 0.f, 0.f };
	glm::vec3 mMinExtent{ 0.f, 0.f, 0.f };
	glm::vec3 mMaxExtent{ 0.f, 0.f, 0.f };
	float mRadiusExtent{ 1.f };
};