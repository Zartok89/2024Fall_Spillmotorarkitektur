#pragma once
#include "utility/VariableTypes.h"

class Texture
{
public:
	/*
	 * Texture construtors, setup and bindings
	 */
	Texture(TextureID textureID, char const* textureFilePath);
	void TextureSetup();
	void BindTextures();

	/*
	 * Member Variables
	 */
	TextureID mTextureID;
	char const* mTextureFailePath;
};
