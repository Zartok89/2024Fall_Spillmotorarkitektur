#include "RandomNumberGenerator.h"

int RandomNumberGenerator::GeneratorRandomNumber(int MinValue, int MaxValue)
{
	// Create a random device to seed the random number generator
	std::random_device RandomDevice;

	// Initialize a random number generator with the seed
	std::mt19937 Generator(RandomDevice());

	// Define a distribution range (e.g., 1 to 100)
	std::uniform_int_distribution<> Distribution(MinValue, MaxValue);

	// Generate and print a random number
	int RandomNumber = Distribution(Generator);

	return RandomNumber;
}

glm::vec3 RandomNumberGenerator::GeneratorRandomVector(int MinValue, int MaxValue)
{
	int N1 = GeneratorRandomNumber(MinValue, MaxValue);
	int N2 = GeneratorRandomNumber(MinValue, MaxValue);
	int N3 = GeneratorRandomNumber(MinValue, MaxValue);
	
	return glm::vec3{ N1, N3, N2 };
}