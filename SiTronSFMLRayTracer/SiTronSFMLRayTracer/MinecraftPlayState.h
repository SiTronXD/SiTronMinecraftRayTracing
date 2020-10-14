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
	static const uint32_t NUM_CHUNK_WIDTH_LENGTH = 8;
	static const uint32_t NUM_CHUNK_HEIGHT = 4;

	static const uint32_t LIGHTMAP_BLOCK_SIDE_SIZE = 32;
	static const uint32_t NUM_MAX_SAMPLES = 1024;
	static const uint32_t NUM_MAX_ITERATIONS_PER_FRAME = 1;

	static const uint8_t LIGHTMAP_UP_HORIZONTAL_TILE_SIZE = 2;
	static const uint8_t LIGHTMAP_UP_VERTICAL_TILE_SIZE = 2;
	static const uint8_t LIGHTMAP_RIGHT_HORIZONTAL_TILE_SIZE = 4;
	static const uint8_t LIGHTMAP_RIGHT_VERTICAL_TILE_SIZE = 2;
	static const uint8_t LIGHTMAP_FRONT_HORIZONTAL_TILE_SIZE = 4;
	static const uint8_t LIGHTMAP_FRONT_VERTICAL_TILE_SIZE = 2;

	sf::Shader rayTracingShader;
	sf::Shader lightmapGeneratorShader;
	sf::Shader lightmapDenoiserShader;
	sf::Shader postProcessingShader;

	sf::Texture textureSheet;
	sf::Texture blueNoiseTexture;
	sf::Texture crosshairTexture;

	sf::RenderTexture renderTexture;
	sf::RenderTexture lightmapTextures[3];

	sf::RectangleShape windowShaderRect; 
	sf::RectangleShape lightmapShaderRect[3];
	sf::RectangleShape crosshairRect;

	SettingsHandler settingsHandler;
	WorldHandler worldHandler;
	InputHandler inputHandler;
	Player player;

	float timer = 0.0f;

	uint32_t currentLightmapIteration = 0;

	bool clearLightmapAfterModification = false;

	sf::Font font;
	sf::Text text;

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