#include "Loader.h"

// Loads a shader from a specified file path
void Loader::loadShader(
    std::string _vertexShaderFilePath, 
	std::string _fragmentShaderFilePath, 
	sf::Shader& _shader)
{
    if (!_shader.loadFromFile(
        _vertexShaderFilePath,
        _fragmentShaderFilePath
    ))
    {
        Log::print("\nCould not load shader from file path: \n" + 
            _vertexShaderFilePath + "\n" + 
            _fragmentShaderFilePath
        );

        Log::pauseConsole();
    }
}

// Loads a texture from a specified file path
void Loader::loadTexture(std::string _textureFilePath, sf::Texture& _texture)
{
    if (!_texture.loadFromFile(_textureFilePath))
    {
        Log::print("\nCould not load texture from file path: \n" + _textureFilePath);

        Log::pauseConsole();
    }
}
