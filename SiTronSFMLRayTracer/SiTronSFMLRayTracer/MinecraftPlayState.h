#pragma once

#include "Gamestate.h"

#include "Loader.h"
#include "Player.h"
#include "WorldHandler.h"
#include "InputHandler.h"
#include "SettingsHandler.h"

class MinecraftPlayState : public Gamestate
{
private:
	static const uint32_t NUM_MAX_RENDER_BLOCKS = 256;

	static const uint32_t LIGHTMAP_SIZE = 512;
	static const uint32_t NUM_MAX_SAMPLES = 2048;
	static const uint32_t NUM_MAX_ITERATIONS_PER_FRAME = 1;

	sf::Shader rayTracingShader;
	sf::Shader lightmapGeneratorShader;
	sf::Shader postProcessingShader;

	sf::Texture textureSheet;
	sf::Texture blueNoiseTexture;
	sf::Texture crosshairTexture;

	sf::RenderTexture renderTexture;
	sf::RenderTexture lightmapTextures[3];

	sf::RectangleShape windowShaderRect; 
	sf::RectangleShape lightmapShaderRect;
	sf::RectangleShape crosshairRect;

	SettingsHandler settingsHandler;
	WorldHandler worldHandler;
	InputHandler inputHandler;
	Player player;

	float timer = 0.0f;

	int currentLightmapIteration = 0;

public:
	MinecraftPlayState(sf::RenderWindow& _window);
	virtual ~MinecraftPlayState();

	virtual void init();
	virtual void handleInput(float dt);
	virtual void update(float dt);
	virtual void draw();

	void clearLightmaps();
	void iterateOverLightmaps();
};