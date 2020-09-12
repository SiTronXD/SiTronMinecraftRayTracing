#pragma once

#include <SFML/Graphics.hpp>

class Hit
{
public:
	Hit();
	~Hit();

	const void set(float _t, sf::Vector3f _hitPoint, sf::Vector3f _hitNormal);

	float t;
	sf::Vector3f point;
	sf::Vector3f normal;

	bool hitSomething();

	static Hit getClosestHit(Hit hit1, Hit hit2);
};