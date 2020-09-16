#include <iostream>
#include <SFML/Graphics.hpp>

#include "GamestateManager.h"
#include "MinecraftPlayState.h"

#include "SettingsHandler.h"

int main()
{
    SettingsHandler settingsHandler;

    sf::RenderWindow window(sf::VideoMode(settingsHandler.GetWindowWidth(), settingsHandler.GetWindowHeight()), "SiTron Ray Tracer In SFML");
    
    GamestateManager gsm(new MinecraftPlayState(window));

    float time = 0.0f;
    sf::Clock deltaClock;

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

        gsm.handleInputState(dt);
        gsm.updateState(dt);
        gsm.drawState();

        // Display
        window.display();
    }

    // Destoy GamestateManager, just to be sure
    //gsm.~GamestateManager();

    // (void) getchar();
    return 0;
}