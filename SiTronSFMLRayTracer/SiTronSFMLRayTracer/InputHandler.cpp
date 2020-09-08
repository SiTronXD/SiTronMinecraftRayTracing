#include "InputHandler.h"

InputHandler::InputHandler(Player& _player)
    : player(_player)
{

}

InputHandler::~InputHandler()
{
}

void InputHandler::Update(const float _dt)
{
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


    // Player controller
    int forwardDir = sf::Keyboard::isKeyPressed(sf::Keyboard::W) - 
        sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    int upDir = sf::Keyboard::isKeyPressed(sf::Keyboard::E) -
        sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
    int rightDir = sf::Keyboard::isKeyPressed(sf::Keyboard::D) -
        sf::Keyboard::isKeyPressed(sf::Keyboard::A);

    player.MovePosition(forwardDir, upDir, rightDir, _dt);


    float horizontalDir = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) -
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    float verticalDir = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) -
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

    horizontalDir *= -2.0 * _dt;
    verticalDir *= 2.0 * _dt;

    player.RotateDirection(horizontalDir, verticalDir);
}
