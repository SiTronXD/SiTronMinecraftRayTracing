#pragma once

#include <SFML/Graphics.hpp>

#include "Log.h"

class Loader
{
public:
	static void loadShader(
		std::string _vertexShaderFilePath, 
		std::string _fragmentShaderFilePath,
		sf::Shader& _shader
	);

	static void loadTexture(
		std::string _textureFilePath,
		sf::Texture& _texture
	);
};