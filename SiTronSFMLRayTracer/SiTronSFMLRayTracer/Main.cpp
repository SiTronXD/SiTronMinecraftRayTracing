#include <iostream>
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "InputHandler.h"

int windowWidth = 1280;
int windowHeight = 720;

Player player;
InputHandler inputHandler(player);

int main()
{
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SiTron Ray Tracer In SFML");
    sf::RectangleShape rect(sf::Vector2f((float) windowWidth, (float) windowHeight));
    rect.setFillColor(sf::Color::Green);

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

        getchar();

        return -1;
    }

    sf::Shader postProcessingShader;
    if (!postProcessingShader.loadFromFile(
        "Resources/Shaders/PostProcessingEffect_Vert.glsl", 
        "Resources/Shaders/PostProcessingEffect_Frag.glsl"
        ))
    {
        std::cout << "Could not load post-processing shaders..." << std::endl;

        getchar();

        return -1;
    }

    sf::Texture textureSheet;
    if (!textureSheet.loadFromFile("Resources/Graphics/minecraftTextureSheet.png"))
    {
        std::cout << "Could not load texture..." << std::endl;

        return -1;
    }

    bool reloadedShader = false;
    float time = 0.0f;
    sf::Clock deltaClock;

    // Textures
    sf::Glsl::Vec4 rects[3];
    rects[0] = sf::Glsl::Vec4(0, 0, 16, 16);    // Dirt block - up
    rects[1] = sf::Glsl::Vec4(48, 0, 16, 16);   // Dirt block - side
    rects[2] = sf::Glsl::Vec4(32, 0, 16, 16);   // Dirt block - down

    // Blocks
    sf::Glsl::Vec3 blockPositions[2];
    blockPositions[0] = sf::Glsl::Vec3(0, 0, 0);
    blockPositions[1] = sf::Glsl::Vec3(2, 0, 0);

    sf::RenderTexture renderTexture;
    if (!renderTexture.create(windowWidth, windowHeight))
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


        float dt = deltaClock.restart().asSeconds();
        time += dt;

        std::cout << "FPS: " << (1.0f / dt) << std::endl;


        inputHandler.Update(dt);
        player.Update(dt);


        // Update shader

        // Camera
        rayTracingShader.setUniform("u_cameraPosition", player.GetPosition());
        rayTracingShader.setUniform("u_cameraForwardDirection", player.GetForwardVector());

        // Blocks
        rayTracingShader.setUniformArray("u_blockTextureRect", rects, 3);
        rayTracingShader.setUniformArray("u_blocks", blockPositions, 2);
        rayTracingShader.setUniform("u_textureSheet", textureSheet);

        // Other shader uniforms
        rayTracingShader.setUniform("u_resolution", sf::Glsl::Vec2((float)windowWidth, (float)windowHeight));
        rayTracingShader.setUniform("u_time", time);


        // Clear window
        //window.clear();

        // Render world to texture
        //renderTexture.clear(sf::Color::Red);
        renderTexture.draw(rect, &rayTracingShader);
        renderTexture.display();

        // Render texture to screen with post-processing effect
        postProcessingShader.setUniform("u_mainTexture", renderTexture.getTexture());
        postProcessingShader.setUniform("u_resolution", sf::Glsl::Vec2((float)windowWidth, (float)windowHeight));
        window.draw(rect, &postProcessingShader);

        // Display
        window.display();
    }

    return 0;
}