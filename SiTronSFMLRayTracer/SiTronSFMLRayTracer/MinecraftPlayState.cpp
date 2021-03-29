#include "MinecraftPlayState.h"

MinecraftPlayState::MinecraftPlayState(sf::RenderWindow& _window)
    : Gamestate(_window), player(nullptr)
{
	
}

MinecraftPlayState::~MinecraftPlayState()
{
    delete this->player;
}

void MinecraftPlayState::init()
{
    // Check for shader availability
    if (!sf::Shader::isAvailable())
    {
        Log::print("Shaders are not available for this gpu...");
        Log::pauseConsole();
    }

    // Load shaders
    Loader::loadShader(
        "Resources/Shaders/RayTracing_Vert.glsl", 
        "Resources/Shaders/Raytracing_Frag.glsl",
        this->rayTracingShader
    );
    Loader::loadShader(
        "Resources/Shaders/LightmapGenerator_Vert.glsl",
        "Resources/Shaders/LightmapGenerator_Frag.glsl",
        this->lightmapGeneratorShader
    );
    Loader::loadShader(
        "Resources/Shaders/PostProcessingEffect_Vert.glsl",
        "Resources/Shaders/PostProcessingEffect_Frag.glsl",
        this->postProcessingShader
    );
    Loader::loadShader(
        "Resources/Shaders/Denoiser_Vert.glsl",
        "Resources/Shaders/Denoiser_Frag.glsl",
        this->lightmapDenoiserShader
    );

    // Load textures
    Loader::loadTexture("Resources/Graphics/textureSheet.png", textureSheet);
    Loader::loadTexture("Resources/Graphics/blueNoiseTexture.png", blueNoiseTexture);
    Loader::loadTexture("Resources/Graphics/Crosshair.png", crosshairTexture);

    // Initialize window shader rect
    windowShaderRect.setSize(sf::Vector2f((float)settingsHandler.GetWindowWidth(), (float)settingsHandler.GetWindowHeight()));
    windowShaderRect.setFillColor(sf::Color::Green);

    // Initialize lightmap shader rects
    lightmapShaderRect[0].setSize(
        sf::Vector2f(
            LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_UP_HORIZONTAL_TILE_SIZE, 
            LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_UP_VERTICAL_TILE_SIZE
        )
    );
    lightmapShaderRect[1].setSize(
        sf::Vector2f(
            LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_RIGHT_HORIZONTAL_TILE_SIZE,
            LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_HEIGHT * LIGHTMAP_RIGHT_VERTICAL_TILE_SIZE
        )
    );
    lightmapShaderRect[2].setSize(
        sf::Vector2f(
            LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_FRONT_HORIZONTAL_TILE_SIZE,
            LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_HEIGHT * LIGHTMAP_FRONT_VERTICAL_TILE_SIZE
        )
    );
    windowShaderRect.setFillColor(sf::Color::Magenta);

    // Create render texture for post processing effects
    if (!screenRenderTexture.create(settingsHandler.GetWindowWidth(), settingsHandler.GetWindowHeight()))
        Log::print("Couldn't create render texture");

    // Create render textures for light maps
    if(!lightmapTextures[0].create(LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_UP_HORIZONTAL_TILE_SIZE, LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_UP_VERTICAL_TILE_SIZE))
        Log::print("Couldn't create render texture for up light map");
    if (!lightmapTextures[1].create(LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_RIGHT_HORIZONTAL_TILE_SIZE, LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_HEIGHT * LIGHTMAP_RIGHT_VERTICAL_TILE_SIZE))
        Log::print("Couldn't create render texture for right light map");
    if (!lightmapTextures[2].create(LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_WIDTH_LENGTH * LIGHTMAP_FRONT_HORIZONTAL_TILE_SIZE, LIGHTMAP_BLOCK_SIDE_SIZE * NUM_CHUNK_HEIGHT * LIGHTMAP_FRONT_VERTICAL_TILE_SIZE))
        Log::print("Couldn't create render texture for front light map");

    player = new Player();
    player->init(&worldHandler);
    inputHandler.init(&*player, &window);

    // Crosshair
    crosshairRect.setSize(sf::Vector2f(25, 25));
    crosshairRect.setPosition(
        settingsHandler.GetWindowWidth() / 2.0f - crosshairRect.getSize().x / 2.0f,
        settingsHandler.GetWindowHeight() / 2.0f - crosshairRect.getSize().y / 2.0f
    );
    crosshairRect.setTexture(&crosshairTexture);


    // Spawn blocks
    for (int x = 0; x < 8; x++)
    {
        for (int z = 0; z < 8; z++)
        {
            float noiseX = x / 1000.0f;
            float noiseZ = z / 1000.0f;
            float y = floor(SMath::perlinNoise(noiseX, noiseZ) * 5.0f) - 4.0f;
            y = SMath::clamp(y, -3, 0);

            worldHandler.AddBlock(
                sf::Vector3i(x, (int) y, z), 
                BlockType::Stone
            );
        }
    }

    for (int z = 0; z < 8; z++)
    {
        for (int y = 0; y > -4; y--)
        {
            worldHandler.AddBlock(sf::Vector3i(0, y, z), BlockType::Stone);
            worldHandler.AddBlock(sf::Vector3i(3, y, z), BlockType::Stone);

            worldHandler.RemoveBlock(sf::Vector3i(1, y, z));
            worldHandler.RemoveBlock(sf::Vector3i(2, y, z));

            if (y == 0 || y == -3)
            {
                worldHandler.AddBlock(sf::Vector3i(1, y, z), BlockType::Stone);
                worldHandler.AddBlock(sf::Vector3i(2, y, z), BlockType::Stone);
            }
        }
    }
    worldHandler.RemoveBlock(sf::Vector3i(1, 0, 2));
    worldHandler.RemoveBlock(sf::Vector3i(2, 0, 2));

    // Redstone blocks to test god rays
    worldHandler.AddBlock(sf::Vector3i(6, -2, 7), BlockType::RedstoneBlock);
    worldHandler.AddBlock(sf::Vector3i(5, -2, 7), BlockType::RedstoneBlock);
    worldHandler.AddBlock(sf::Vector3i(6, -1, 7), BlockType::RedstoneBlock);
    worldHandler.AddBlock(sf::Vector3i(5, -1, 7), BlockType::RedstoneBlock);

    worldHandler.AddBlock(sf::Vector3i(7, -2, 6), BlockType::RedstoneBlock);
    worldHandler.AddBlock(sf::Vector3i(7, -2, 5), BlockType::RedstoneBlock);
    worldHandler.AddBlock(sf::Vector3i(7, -1, 5), BlockType::RedstoneBlock);

    worldHandler.AddBlock(sf::Vector3i(6, 0, 6), BlockType::RedstoneBlock);
    worldHandler.AddBlock(sf::Vector3i(5, 0, 6), BlockType::RedstoneBlock);

    // Sun setup 1
    this->sunPos = sf::Vector3f(0.0f, 4.0f, 0.0f);
    //this->sunColor = sf::Vector3f(1.0f, 0.2f, 0.2f);
    this->sunColor = sf::Vector3f(1.0f, 0.9f, 0.8f);
    this->sunRadius = 3.0f;
    this->sunColorIntensity = 3.0f;
    
    // Sun setup 2
    /*this->sunPos = sf::Vector3f(0.0f, 4.0f, 4.0f);
    this->sunColor = sf::Vector3f(0.2f, 1.0f, 0.2f);
    this->sunRadius = 1.0f;
    this->sunColorIntensity = 30.0f;*/

    // Text
    font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::Red);

    // Clear lightmaps
    if (this->generateLightmaps)
        this->clearLightmaps();
    else
        this->clearLightmapsToWhite();
}

void MinecraftPlayState::handleInput(float dt)
{
    inputHandler.Update(dt);

    if (inputHandler.playerModifiedWorld() && clearLightmapAfterModification)
        this->clearLightmaps();
}

void MinecraftPlayState::update(float dt)
{
    timer += dt;

    // Find all blocks to render
    std::vector<Block*> blocksToRender = worldHandler.GetBlocksToRender();

    // Fill arrays with positions, indices and specular
    int numValidBlocks = (int) SMath::min(256, (float) blocksToRender.size());

    sf::Glsl::Vec3 blockPositions[NUM_MAX_RENDER_BLOCKS] {};    // Positions for each block

    // X: Index for each block
    // Y: Specular for each block
    // Z: Transparency for each block
    sf::Glsl::Vec3 blockInfo[NUM_MAX_RENDER_BLOCKS] {};

    // Find and pack information
    for (int i = 0; i < numValidBlocks; i++)
    {
        blockPositions[i] = (sf::Glsl::Vec3) blocksToRender[i]->getPosition();

        // Change alpha over time
        float o = blocksToRender[i]->getBlockTypeIndex() != 0 ? 1.0f :
            (sin(timer) * 0.5f + 0.5f);

        // Pack vec3 with information
        blockInfo[i] = sf::Glsl::Vec3(
            static_cast<float>(blocksToRender[i]->getBlockTypeIndex()), 
            blocksToRender[i]->getBlockSpecular(),
            blocksToRender[i]->getBlockTransparency() * o
        );
    }

    // Package camera vectors into camera matrix
    sf::Glsl::Vec3 camRight = player->getRightVector();
    sf::Glsl::Vec3 camUp = player->getUpVector();
    sf::Glsl::Vec3 camForward = player->getForwardVector();

    float cameraRotMatFloatArray[3 * 3] =
    {
        camRight.x, camRight.y, camRight.z,
        camUp.x, camUp.y, camUp.z,
        camForward.x, camForward.y, camForward.z
    };
    sf::Glsl::Mat3 cameraRot(cameraRotMatFloatArray);


    // Update shader

    // Camera
    rayTracingShader.setUniform("u_cameraPosition", player->getPosition());
    rayTracingShader.setUniform("u_cameraRot", cameraRot);

    // Sun
	rayTracingShader.setUniform("u_sunSpherePosRadius", 
        sf::Glsl::Vec4(
            sunPos.x,
            sunPos.y,
            sunPos.z,
            sunRadius
	    )
    );
	rayTracingShader.setUniform("u_sunColor", 
        sf::Glsl::Vec3(
            sunColor.x,
            sunColor.y,
            sunColor.z
	    )
    );

    // Blocks
    rayTracingShader.setUniformArray("u_blockTextureRect", Block::TEXTURE_RECTS, Block::MAX_NUM_TEXTURE_RECTS);
    rayTracingShader.setUniformArray("u_blocks", blockPositions, NUM_MAX_RENDER_BLOCKS);
    rayTracingShader.setUniformArray("u_blockInfo", blockInfo, NUM_MAX_RENDER_BLOCKS);

    rayTracingShader.setUniform("u_numValidBlocks", numValidBlocks);
    rayTracingShader.setUniform("u_textureSheet", textureSheet);
    rayTracingShader.setUniform("u_blueNoiseTexture", blueNoiseTexture);
    rayTracingShader.setUniform("u_lightMapUpTexture", lightmapTextures[0].getTexture());
    rayTracingShader.setUniform("u_lightMapRightTexture", lightmapTextures[1].getTexture());
    rayTracingShader.setUniform("u_lightMapFrontTexture", lightmapTextures[2].getTexture());

    // Other shader uniforms
    rayTracingShader.setUniform("u_resolution", sf::Glsl::Vec2((float)settingsHandler.GetWindowWidth(), (float)settingsHandler.GetWindowHeight()));
    rayTracingShader.setUniform("u_time", timer);
}

void MinecraftPlayState::draw()
{
    this->iterateOverLightmaps();

    // Render world to texture
    screenRenderTexture.draw(windowShaderRect, &rayTracingShader);
    screenRenderTexture.draw(crosshairRect);
    screenRenderTexture.display();

    // Render texture to screen with post-processing effect
    postProcessingShader.setUniform("u_mainTexture", screenRenderTexture.getTexture());
    postProcessingShader.setUniform("u_resolution", sf::Glsl::Vec2((float)settingsHandler.GetWindowWidth(), (float)settingsHandler.GetWindowHeight()));
    window.draw(windowShaderRect, &postProcessingShader);


    // Render lightmaps calculated text
    float percent = (float)this->currentLightmapIteration / NUM_MAX_SAMPLES * 100;
    if (percent < 100.0f)
	{
		text.setString("Calculating lightmaps: " +
			std::to_string(percent) + "%");

        window.draw(text);
    }
}

void MinecraftPlayState::clearLightmaps()
{
	this->currentLightmapIteration = 0;

    for(int i = 0; i < 3; i++)
	    this->lightmapTextures[i].clear(sf::Color::Black);
}

void MinecraftPlayState::clearLightmapsToWhite()
{
	this->currentLightmapIteration = this->NUM_MAX_SAMPLES;

    for(int i = 0; i < 3; i++)
	    this->lightmapTextures[i].clear(sf::Color::White);
}

void MinecraftPlayState::iterateOverLightmaps()
{
    // Reached max num of samples
    if (this->currentLightmapIteration >= NUM_MAX_SAMPLES)
        return;

    // Collect info
    std::vector<Block*> blocksToRender = worldHandler.GetBlocksToRender();
    int numValidBlocks = (int) SMath::min(256, (float) blocksToRender.size());

    sf::Glsl::Vec3 blockPositions[NUM_MAX_RENDER_BLOCKS]{};	    // Positions for each block

    for (int i = 0; i < numValidBlocks; i++)
    {
        blockPositions[i] = (sf::Glsl::Vec3) blocksToRender[i]->getPosition();
    }

    // Update shader
    this->lightmapGeneratorShader.setUniform("u_lightmapSideSize", (int)this->LIGHTMAP_BLOCK_SIDE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_numValidBlocks", numValidBlocks);
    this->lightmapGeneratorShader.setUniformArray("u_blocks", blockPositions, this->NUM_MAX_RENDER_BLOCKS);

	// Sun
    this->lightmapGeneratorShader.setUniform("u_sunSpherePosRadius", 
        sf::Glsl::Vec4
        (
            sunPos.x,
            sunPos.y,
            sunPos.z,
            sunRadius
        )
    );
    this->lightmapGeneratorShader.setUniform("u_sunColor",
        sf::Glsl::Vec3
        (
            sunColor.x * sunColorIntensity,
            sunColor.y * sunColorIntensity,
            sunColor.z * sunColorIntensity
        )
    );

    sf::RenderStates currentRenderState;
    currentRenderState.blendMode = sf::BlendNone;
    currentRenderState.shader = &this->lightmapGeneratorShader;

    // Update shader
    this->lightmapGeneratorShader.setUniform("u_currentIteration", (int) this->currentLightmapIteration);

    // ---Up sides---
    this->lightmapGeneratorShader.setUniform("u_currentSide", 0);
    this->lightmapGeneratorShader.setUniform("u_lightmapNumHorizontalTiles", (int)this->LIGHTMAP_UP_HORIZONTAL_TILE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_lightmapNumVerticalTiles", (int)this->LIGHTMAP_UP_VERTICAL_TILE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_lastFrameTexture", this->lightmapTextures[0].getTexture());

    // Generate lightmaps
    this->lightmapTextures[0].draw(this->lightmapShaderRect[0], currentRenderState);
    this->lightmapTextures[0].display();

    // ---Right sides---
    this->lightmapGeneratorShader.setUniform("u_currentSide", 1);
    this->lightmapGeneratorShader.setUniform("u_lightmapNumHorizontalTiles", (int)this->LIGHTMAP_RIGHT_HORIZONTAL_TILE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_lightmapNumVerticalTiles", (int)this->LIGHTMAP_RIGHT_VERTICAL_TILE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_lastFrameTexture", this->lightmapTextures[1].getTexture());

    // Generate lightmaps
    this->lightmapTextures[1].draw(this->lightmapShaderRect[1], currentRenderState);
    this->lightmapTextures[1].display();


    // ---Front sides---
    this->lightmapGeneratorShader.setUniform("u_currentSide", 2);
    this->lightmapGeneratorShader.setUniform("u_lightmapNumHorizontalTiles", (int)this->LIGHTMAP_FRONT_HORIZONTAL_TILE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_lightmapNumVerticalTiles", (int)this->LIGHTMAP_FRONT_VERTICAL_TILE_SIZE);
    this->lightmapGeneratorShader.setUniform("u_lastFrameTexture", this->lightmapTextures[2].getTexture());

    // Generate lightmaps
    this->lightmapTextures[2].draw(this->lightmapShaderRect[2], currentRenderState);
    this->lightmapTextures[2].display();

    this->currentLightmapIteration++;


    // The last sample has been reached
    if (this->currentLightmapIteration >= NUM_MAX_SAMPLES)
    {
        // Find current size
        for (int i = 0; i < 3; i++)
        {
            float lmChunkSizeWidth = 0;
            float lmChunkSizeHeight = 0;
            float lmNumTilesWidth = 0;
            float lmNumTilesHeight = 0;
            float lmWidth = 0;
            float lmHeight = 0;

            switch (i)
            {
            case 0:
                lmChunkSizeWidth = NUM_CHUNK_WIDTH_LENGTH;
                lmChunkSizeHeight = NUM_CHUNK_WIDTH_LENGTH;

                lmNumTilesWidth = LIGHTMAP_UP_HORIZONTAL_TILE_SIZE;
                lmNumTilesHeight = LIGHTMAP_UP_VERTICAL_TILE_SIZE;

                lmWidth = LIGHTMAP_UP_HORIZONTAL_TILE_SIZE * NUM_CHUNK_WIDTH_LENGTH;
                lmHeight = LIGHTMAP_UP_VERTICAL_TILE_SIZE * NUM_CHUNK_WIDTH_LENGTH;
                break;
            case 1:
                lmChunkSizeWidth = NUM_CHUNK_WIDTH_LENGTH;
                lmChunkSizeHeight = NUM_CHUNK_HEIGHT;

                lmNumTilesWidth = LIGHTMAP_RIGHT_HORIZONTAL_TILE_SIZE;
                lmNumTilesHeight = LIGHTMAP_RIGHT_VERTICAL_TILE_SIZE;

                lmWidth = LIGHTMAP_RIGHT_HORIZONTAL_TILE_SIZE * NUM_CHUNK_WIDTH_LENGTH;
                lmHeight = LIGHTMAP_RIGHT_VERTICAL_TILE_SIZE * NUM_CHUNK_HEIGHT;
                break;
            case 2:
                lmChunkSizeWidth = NUM_CHUNK_WIDTH_LENGTH;
                lmChunkSizeHeight = NUM_CHUNK_HEIGHT;

                lmNumTilesWidth = LIGHTMAP_FRONT_HORIZONTAL_TILE_SIZE;
                lmNumTilesHeight = LIGHTMAP_FRONT_VERTICAL_TILE_SIZE;

                lmWidth = LIGHTMAP_FRONT_HORIZONTAL_TILE_SIZE * NUM_CHUNK_WIDTH_LENGTH;
                lmHeight = LIGHTMAP_FRONT_VERTICAL_TILE_SIZE * NUM_CHUNK_HEIGHT;
                break;
            }

            lmWidth *= LIGHTMAP_BLOCK_SIDE_SIZE;
            lmHeight *= LIGHTMAP_BLOCK_SIDE_SIZE;

            // Set uniforms
            this->lightmapDenoiserShader.setUniform("u_shouldBlur", false);
            this->lightmapDenoiserShader.setUniform("u_lightmapNumChunkSize", sf::Glsl::Vec2(lmChunkSizeWidth, lmChunkSizeHeight));
            this->lightmapDenoiserShader.setUniform("u_lightmapNumTiles", sf::Glsl::Vec2(lmNumTilesWidth, lmNumTilesHeight));
            this->lightmapDenoiserShader.setUniform("u_lightmapSize", sf::Glsl::Vec2(lmWidth, lmHeight));
            this->lightmapDenoiserShader.setUniform("u_lightmapTexture", this->lightmapTextures[i].getTexture());

            // Render
            this->lightmapTextures[i].draw(this->lightmapShaderRect[i], &this->lightmapDenoiserShader);
            this->lightmapTextures[i].display();
        }
    }
}