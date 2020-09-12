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
    sf::Glsl::Vec4 rects[3];
    rects[0] = sf::Glsl::Vec4(0, 0, 16, 16);    // Dirt block - up
    rects[1] = sf::Glsl::Vec4(48, 0, 16, 16);   // Dirt block - side
    rects[2] = sf::Glsl::Vec4(32, 0, 16, 16);   // Dirt block - down

    // Blocks
    //worldHandler.AddBlock(sf::Vector3i(0, 0, 0), BlockType::Grass);
    worldHandler.AddBlock(sf::Vector3i(2, 0, 0), BlockType::Grass);

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

        //std::cout << "FPS: " << (1.0f / dt) << std::endl;


        inputHandler.Update(dt);
        player.Update(dt);

        // Find all blocks to render
        std::vector<sf::Vector3i> blocksToRender = worldHandler.GetBlocksToRender();

        const int maxBlockPositions = 256;
        sf::Glsl::Vec3 blockPositions[maxBlockPositions];
        float blockIsValid[maxBlockPositions];
        for (int i = 0; i < maxBlockPositions; i++)
        {
            blockIsValid[i] = i < blocksToRender.size();
            if (!blockIsValid[i])
                continue;

            blockPositions[i] = (sf::Glsl::Vec3) blocksToRender[i];
        }

        // Package camera vectors into camera matrix
        sf::Glsl::Vec3 camRight = player.GetRightVector();
        sf::Glsl::Vec3 camUp = player.GetUpVector();
        sf::Glsl::Vec3 camForward = player.GetForwardVector();

        float cameraRotMatFloatArray[3 * 3] =
        {
            camRight.x, camRight.y, camRight.z,
            camUp.x, camUp.y, camUp.z,
            camForward.x, camForward.y, camForward.z
        };
        sf::Glsl::Mat3 cameraRot(cameraRotMatFloatArray);


        // Update shader

        // Camera
        rayTracingShader.setUniform("u_cameraPosition", player.GetPosition());
        rayTracingShader.setUniform("u_cameraRot", cameraRot);

        // Blocks
        rayTracingShader.setUniformArray("u_blockIsValid", blockIsValid, maxBlockPositions);
        rayTracingShader.setUniformArray("u_blockTextureRect", rects, 3);
        rayTracingShader.setUniformArray("u_blocks", blockPositions, maxBlockPositions);
        rayTracingShader.setUniform("u_textureSheet", textureSheet);

        // Other shader uniforms
        rayTracingShader.setUniform("u_resolution", sf::Glsl::Vec2((float) settingsHandler.GetWindowWidth(), (float) settingsHandler.GetWindowHeight()));
        rayTracingShader.setUniform("u_time", time);


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