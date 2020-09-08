#include "InputHandler.h"

InputHandler::InputHandler(Player& _player, sf::Window& _window)
    : player(_player), window(_window)
{
    screenMiddlePos = sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
    lastMousePosition = sf::Mouse::getPosition();

    lockMouse = true;
    lastPressedTab = false;
    window.setMouseCursorVisible(!lockMouse);
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

    // Lock mouse
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && !lastPressedTab)
    {
        lockMouse = !lockMouse;

        window.setMouseCursorVisible(!lockMouse);
    }
    lastPressedTab = sf::Keyboard::isKeyPressed(sf::Keyboard::Tab);

    // Player input for movement
    int forwardDir = sf::Keyboard::isKeyPressed(sf::Keyboard::W) - 
        sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    int upDir = sf::Keyboard::isKeyPressed(sf::Keyboard::E) -
        sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
    int rightDir = sf::Keyboard::isKeyPressed(sf::Keyboard::D) -
        sf::Keyboard::isKeyPressed(sf::Keyboard::A);

    // Move player
    player.MovePosition(forwardDir, upDir, rightDir, _dt);


    // Player mouse look
    if (lockMouse)
    {
        sf::Vector2f deltaMousePosition = (sf::Vector2f) (lastMousePosition - sf::Mouse::getPosition());
        deltaMousePosition *= 0.003f;

        // Set mouse position to the middle of the window
        sf::Vector2i newMousePos = screenMiddlePos;
        sf::Mouse::setPosition(newMousePos);

        // Update last mouse position
        lastMousePosition = sf::Mouse::getPosition();

        // Rotate player
        player.RotateDirection(deltaMousePosition.x, deltaMousePosition.y);
    }
}
