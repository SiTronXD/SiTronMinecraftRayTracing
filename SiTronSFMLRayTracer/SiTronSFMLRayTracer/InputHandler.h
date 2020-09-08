#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "SettingsHandler.h"

class InputHandler
{
private:
	Player& player;
	sf::Window& window;

	sf::Vector2i screenMiddlePos;
	sf::Vector2i lastMousePosition;

	bool lockMouse;
	bool lastPressedTab;

public:
	InputHandler(Player& _player, sf::Window& _window);
	~InputHandler();

	void Update(const float _dt);
};