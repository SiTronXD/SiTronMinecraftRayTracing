#include <iostream>
#include <SFML/Graphics.hpp>

#include "SettingsHandler.h"
#include "Player.h"
#include "WorldHandler.h"
#include "InputHandler.h"

int main()
{
    SettingsHandler settingsHandler;

    sf::RenderWindow window(sf::VideoMode(settingsHandler.GetWindowWidth(), settingsHandler.GetWindowHeight()), "SiTron Ray Tracer In SFML");
    sf::RectangleShape rect(sf::Vector2f((float)settingsHandler.GetWindowWidth(), (float) settingsHandler.GetWindowHeight()));
    
    rect.setFillColor(sf::Color::Green);

    WorldHandler worldHandler;
    Player player(worldHandler);
    InputHandler inputHandler(player, window);


    // Check for shader availability
    if (!sf::Shader::isAvailable())
    {
        std::cout << "Shaders are not available for this gpu..." << std::endl;

        return -1;
    }

    // Load shaders
    sf::Shader rayTracingShader;
    if (!rayTracingShader.loadFromFile(
        "Resources/Shaders/RayTracing_Vert.glsl",
        "Resources/Shaders/Raytracing_Frag.glsl"
        ))
    {
        std::cout << "Could not load ray tracing shaders..." << std::endl;

        (void)getchar();

        return -1;
    }

    sf::Shader postProcessingShader;
    if (!postProcessingShader.loadFromFile(
        "Resources/Shaders/PostProcessingEffect_Vert.glsl", 
        "Resources/Shaders/PostProcessingEffect_Frag.glsl"
        ))
    {
        std::cout << "Could not load post-processing shaders..." << std::endl;

        (void)getchar();

        return -1;
    }

    sf::Texture textureSheet;
    if (!textureSheet.loadFromFile("Resources/Graphics/minecraftTextureSheet.png"))
    {
        std::cout << "Could not load texture..." << std::endl;

        return -1;
    }

    sf::Texture crosshairTexture;
    if (!crosshairTexture.loadFromFile("Resources/Graphics/Crosshair.png"))
    {
        std::cout << "Could not load texture..." << std::endl;

        return -1;
    }
    sf::RectangleShape crosshairRect(sf::Vector2f(25, 25));
    crosshairRect.setPosition(
        settingsHandler.GetWindowWidth() / 2.0f - crosshairRect.getSize().x / 2.0f,
        settingsHandler.GetWindowHeight() / 2.0f - crosshairRect.getSize().y / 2.0f
    );
    crosshairRect.setTexture(&crosshairTexture);

    bool reloadedShader = false;
    float time = 0.0f;
    sf::Clock deltaClock;

    // Textures
    /*const int MAX_NUM_TEXTURE_RECTS = 9;
    sf::Glsl::Vec4 rects[MAX_NUM_TEXTURE_RECTS];
    rects[3*0 + 0] = sf::Glsl::Vec4(0, 0, 16, 16);    // Dirt block - up
    rects[3*0 + 1] = sf::Glsl::Vec4(48, 0, 16, 16);   // Dirt block - side
    rects[3*0 + 2] = sf::Glsl::Vec4(32, 0, 16, 16);   // Dirt block - down

    rects[3*1 + 0] = sf::Glsl::Vec4(16, 0, 16, 16);   // Stone block - up
    rects[3*1 + 1] = sf::Glsl::Vec4(16, 0, 16, 16);   // Stone block - side
    rects[3*1 + 2] = sf::Glsl::Vec4(16, 0, 16, 16);   // Stone block - down

    rects[3*2 + 0] = sf::Glsl::Vec4(16 * 10, 16, 16, 16);   // Mirror block - up
    rects[3*2 + 1] = sf::Glsl::Vec4(16 * 10, 16, 16, 16);   // Mirror block - side
    rects[3*2 + 2] = sf::Glsl::Vec4(16 * 10, 16, 16, 16);   // Mirror block - down*/

    // Blocks
    for (int x = 0; x < 8; x++)
    {
        for (int z = 0; z < 8; z++)
        {
            float noiseX = x / 1000.0f;
            float noiseZ = z / 1000.0f;
            float y = (float) floor(SMath::perlinNoise(noiseX, noiseZ) * 5.0f) - 2;

            worldHandler.AddBlock(sf::Vector3i(x - 8, y, z - 8), BlockType::Stone);
        }
    }


    sf::RenderTexture renderTexture;
    if (!renderTexture.create(settingsHandler.GetWindowWidth(), settingsHandler.GetWindowHeight()))
    {
        std::cout << "Couldn't create render texture" << std::endl;

        return -1;
    }

    // Game Loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        // Show fps
        float dt = deltaClock.restart().asSeconds();
        time += dt;

        std::cout << "FPS: " << (1.0f / dt) << std::endl;


        inputHandler.Update(dt);

        // Find all blocks to render
        std::vector<Block*> blocksToRender = worldHandler.GetBlocksToRender();

        const int maxBlocks = 256;
        sf::Glsl::Vec3 blockPositions[maxBlocks];
        float blockIndices[maxBlocks];
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
        rayTracingShader.setUniformArray("u_blocks", blockPositions, maxBlocks);
        rayTracingShader.setUniformArray("u_blockIndex", blockIndices, maxBlocks);
        rayTracingShader.setUniform("u_numValidBlocks", numValidBlocks);
        rayTracingShader.setUniform("u_textureSheet", textureSheet);

        // Other shader uniforms
        rayTracingShader.setUniform("u_resolution", sf::Glsl::Vec2((float) settingsHandler.GetWindowWidth(), (float) settingsHandler.GetWindowHeight()));
        //rayTracingShader.setUniform("u_time", time);


        // Clear window
        //window.clear();

        // Render world to texture
        //renderTexture.clear(sf::Color::Red);
        renderTexture.draw(rect, &rayTracingShader);
        renderTexture.draw(crosshairRect);
        renderTexture.display();


        // Render texture to screen with post-processing effect
        postProcessingShader.setUniform("u_mainTexture", renderTexture.getTexture());
        postProcessingShader.setUniform("u_resolution", sf::Glsl::Vec2((float) settingsHandler.GetWindowWidth(), (float) settingsHandler.GetWindowHeight()));
        window.draw(rect, &postProcessingShader);

        // Display
        window.display();
    }

    return 0;
}