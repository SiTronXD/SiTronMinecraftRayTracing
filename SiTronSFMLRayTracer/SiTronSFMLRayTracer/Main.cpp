#include <iostream>

#include <SFML/Graphics.hpp>

int windowWidth = 1280;
int windowHeight = 720;

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

    // Load shader
    sf::Shader shader;
    if (!shader.loadFromFile("Resources/Shaders/RayTracing_Vert.glsl", "Resources/Shaders/Raytracing_Frag.glsl"))
    {
        std::cout << "Could not load ray tracing shaders..." << std::endl;

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

    // Game Loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window
            if (event.type == sf::Event::Closed)
                window.close();

            // Reload shader
            /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !reloadedShader)
            {
                reloadedShader = true;

                std::cout << "Reloading shader..." << std::endl;
                if (!shader.loadFromFile("Resources/Shaders/RayTracing_Vert.glsl", "Resources/Shaders/Raytracing_Frag.glsl"))
                {
                    std::cout << "Could not load ray tracing shaders..." << std::endl;

                    return -1;
                }
            }
            else if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)))
            {
                reloadedShader = false;
            }*/
        }

        float dt = deltaClock.restart().asSeconds();
        time += dt;

        std::cout << "FPS: " << (1.0f / dt) << std::endl;

        // Update shader
        shader.setUniformArray("u_blockTextureRect", rects, 3);
        shader.setUniformArray("u_blocks", blockPositions, 2);
        shader.setUniform("u_textureSheet", textureSheet);
        shader.setUniform("u_resolution", sf::Glsl::Vec2((float)windowWidth, (float)windowHeight));
        shader.setUniform("u_time", time);

        // Clear
        window.clear();

        window.draw(rect, &shader);


        // Display
        window.display();
    }

    return 0;
}