#pragma once

#include <SFML/Graphics.hpp>

class Gamestate
{
protected:
	sf::RenderWindow& window;

public:
	Gamestate(sf::RenderWindow& _window);
	virtual ~Gamestate();

	virtual void init() = 0;
	virtual void handleInput(float dt) = 0;
	virtual void update(float dt) = 0;
	virtual void draw() = 0;
};