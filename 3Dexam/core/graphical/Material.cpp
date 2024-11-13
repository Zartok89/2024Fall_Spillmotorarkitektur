#include "Material.h"

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	mAmbient = ambient;
	mDiffuse = diffuse;
	mSpecular = specular;
	mShininess = shininess;
}

void Material::MaterialSetup(Shader* shader)
{
	shader->setVec3("material.ambient", mAmbient);
	shader->setVec3("material.diffuse", mDiffuse);
	shader->setVec3("material.specular", mSpecular);
	shader->setFloat("material.shininess", mShininess);
}