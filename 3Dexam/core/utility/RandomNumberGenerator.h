#pragma once

#include <iostream>
#include <random>
#include <glm/glm.hpp>

class RandomNumberGenerator
{
public:

	RandomNumberGenerator() = default;
	~RandomNumberGenerator() = default;

	int GeneratorRandomNumber(int MinValue, int MaxValue);
	glm::vec3 GeneratorRandomVector(int MinValue, int MaxValue);
};
