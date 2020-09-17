#include "MinecraftPlayState.h"

MinecraftPlayState::MinecraftPlayState(sf::RenderWindow& _window)
    : Gamestate(_window)
{
	
}

MinecraftPlayState::~MinecraftPlayState()
{
	
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
        rayTracingShader
    );
    Loader::loadShader(
        "Resources/Shaders/PostProcessingEffect_Vert.glsl",
        "Resources/Shaders/PostProcessingEffect_Frag.glsl",
        postProcessingShader
    );

    // Load textures
    Loader::loadTexture("Resources/Graphics/minecraftTextureSheet.png", textureSheet);
    Loader::loadTexture("Resources/Graphics/Crosshair.png", crosshairTexture);

    // Create render texture
    if (!renderTexture.create(settingsHandler.GetWindowWidth(), settingsHandler.GetWindowHeight()))
        Log::print("Couldn't create render texture");


    // Initialize window shader rect
    windowShaderRect.setSize(sf::Vector2f((float)settingsHandler.GetWindowWidth(), (float)settingsHandler.GetWindowHeight()));
    windowShaderRect.setFillColor(sf::Color::Green);

    player.init(&worldHandler);
    inputHandler.init(&player, &window);

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
            float y = (float)floor(SMath::perlinNoise(noiseX, noiseZ) * 5.0f) - 2;

            worldHandler.AddBlock(sf::Vector3i(x - 8, y, z - 8), BlockType::Stone);
        }
    }
}

void MinecraftPlayState::handleInput(float dt)
{
    inputHandler.Update(dt);
}

void MinecraftPlayState::update(float dt)
{
    // Find all blocks to render
    std::vector<Block*> blocksToRender = worldHandler.GetBlocksToRender();

    // Fill arrays with positions and indices
    int numValidBlocks = SMath::min(256, blocksToRender.size());
    for (int i = 0; i < numValidBlocks; i++)
    {
        blockPositions[i] = (sf::Glsl::Vec3) blocksToRender[i]->getPosition();
        blockIndices[i] = blocksToRender[i]->getBlockTypeIndex();
    }

    // Package camera vectors into camera matrix
    sf::Glsl::Vec3 camRight = player.getRightVector();
    sf::Glsl::Vec3 camUp = player.getUpVector();
    sf::Glsl::Vec3 camForward = player.getForwardVector();

    float cameraRotMatFloatArray[3 * 3] =
    {
        camRight.x, camRight.y, camRight.z,
        camUp.x, camUp.y, camUp.z,
        camForward.x, camForward.y, camForward.z
    };
    sf::Glsl::Mat3 cameraRot(cameraRotMatFloatArray);


    // Update shader

    // Camera
    rayTracingShader.setUniform("u_cameraPosition", player.getPosition());
    rayTracingShader.setUniform("u_cameraRot", cameraRot);

    // Blocks
    rayTracingShader.setUniformArray("u_blockTextureRect", Block::textureRects, Block::MAX_NUM_TEXTURE_RECTS);
    rayTracingShader.setUniformArray("u_blocks", blockPositions, NUM_MAX_BLOCKS);
    rayTracingShader.setUniformArray("u_blockIndex", blockIndices, NUM_MAX_BLOCKS);
    rayTracingShader.setUniform("u_numValidBlocks", numValidBlocks);
    rayTracingShader.setUniform("u_textureSheet", textureSheet);

    // Other shader uniforms
    rayTracingShader.setUniform("u_resolution", sf::Glsl::Vec2((float)settingsHandler.GetWindowWidth(), (float)settingsHandler.GetWindowHeight()));
    //rayTracingShader.setUniform("u_time", time);
}

void MinecraftPlayState::draw()
{
    // Render world to texture
    renderTexture.draw(windowShaderRect, &rayTracingShader);
    renderTexture.draw(crosshairRect);
    renderTexture.display();

    // Render texture to screen with post-processing effect
    postProcessingShader.setUniform("u_mainTexture", renderTexture.getTexture());
    postProcessingShader.setUniform("u_resolution", sf::Glsl::Vec2((float)settingsHandler.GetWindowWidth(), (float)settingsHandler.GetWindowHeight()));
    window.draw(windowShaderRect, &postProcessingShader);
}