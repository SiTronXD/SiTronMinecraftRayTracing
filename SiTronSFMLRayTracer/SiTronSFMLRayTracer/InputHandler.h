#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Player.h"

class InputHandler
{
private:
	Player& player;

public:
	InputHandler(Player& _player);
	~InputHandler();

	void Update(const float _dt);
};