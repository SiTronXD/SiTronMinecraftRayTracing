#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "SettingsHandler.h"

class InputHandler
{
private:
	Player* player;
	sf::Window* window;

	sf::Vector2i screenMiddlePos;
	sf::Vector2i lastMousePosition;

	bool lockMouse;
	bool lastPressedTab;
	bool lastPressedMouse1;
	bool lastPressedMouse2;
	bool hasPlayerModifiedWorld;

public:
	InputHandler();
	~InputHandler();

	void init(Player* _player, sf::Window* _window);

	void Update(const float _dt);

	bool playerModifiedWorld() const;
};