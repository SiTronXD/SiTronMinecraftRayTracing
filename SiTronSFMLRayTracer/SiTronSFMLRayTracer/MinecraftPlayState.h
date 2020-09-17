#pragma once

#include "Gamestate.h"

#include "Loader.h"
#include "Player.h"
#include "WorldHandler.h"
#include "InputHandler.h"

class MinecraftPlayState : public Gamestate
{
private:
	sf::Shader rayTracingShader;
	sf::Shader postProcessingShader;

	sf::Texture textureSheet;
	sf::Texture crosshairTexture;

	sf::RenderTexture renderTexture;

	sf::RectangleShape windowShaderRect; 
	sf::RectangleShape crosshairRect;

	SettingsHandler settingsHandler;
	WorldHandler worldHandler;
	InputHandler inputHandler;
	Player player;

	static const int NUM_MAX_BLOCKS = 256;
	sf::Glsl::Vec3 blockPositions[NUM_MAX_BLOCKS];   // Positions for each block
	float blockIndices[NUM_MAX_BLOCKS];              // Index for each block

public:
	MinecraftPlayState(sf::RenderWindow& _window);
	virtual ~MinecraftPlayState();

	virtual void init();
	virtual void handleInput(float dt);
	virtual void update(float dt);
	virtual void draw();
};