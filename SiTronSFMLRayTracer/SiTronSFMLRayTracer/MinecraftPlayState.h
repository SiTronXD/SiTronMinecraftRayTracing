#pragma once

#include <iostream>

#include "Gamestate.h"

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

public:
	MinecraftPlayState(sf::RenderWindow& _window);
	virtual ~MinecraftPlayState();

	virtual void init();
	virtual void handleInput(float dt);
	virtual void update(float dt);
	virtual void draw();
};