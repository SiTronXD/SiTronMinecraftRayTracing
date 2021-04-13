#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include "Hit.h"

class Ray
{
private:
	sf::Vector3f position;
	sf::Vector3f direction;
	sf::Vector3f oneOverDirection;

	Hit hitInfo;

public:
	Ray(sf::Vector3f _position, sf::Vector3f _direction);
	~Ray();

	Hit GetClosestBoxHit(sf::Vector3i boxPos);
};