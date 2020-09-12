#pragma once

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <math.h>

class SMath
{
private:
	static float lerp(const float a, const float b, float t);
	static float smoothstep(const float a, const float b, float t);
	static float fract(float num);
	static float hashNoise(float x, float y);
	static float perlinNoiseLayer(const float x, const float y);

public:
	static float clamp(const float _val, const float _min, const float _max);

	static void vectorCross(const sf::Vector3f& vectorA, const sf::Vector3f& vectorB, sf::Vector3f& resultVector);
	static void vectorNormalize(sf::Vector3f& vector);

	static float perlinNoise(
		const float x, 
		const float y = 0.0f
	);
};