#pragma once

#include <SFML/Graphics.hpp>

class SMath
{
public:
	static float clamp(const float _val, const float _min, const float _max);

	static void vectorCross(const sf::Vector3f& vectorA, const sf::Vector3f& vectorB, sf::Vector3f& resultVector);
	static void vectorNormalize(sf::Vector3f& vector);
};