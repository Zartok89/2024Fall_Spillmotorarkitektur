#pragma once
#include <glm/vec3.hpp>
#include "shader/Shader.h"

class Material
{
public:
	/*
	* Material Constructor
	*/
	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

	/*
	 * Material Setup
	 */
	void MaterialSetup(Shader* shader);

	/*
	 * Member variables
	 */
	glm::vec3 mAmbient{ 1.f, 1.f, 1.f };
	glm::vec3 mDiffuse{ 1.f, 1.f, 1.f };
	glm::vec3 mSpecular{ 1.f, 1.f, 1.f };
	float mShininess{ 1.f };
};
