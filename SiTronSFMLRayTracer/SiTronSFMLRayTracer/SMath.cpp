#include "SMath.h"

float SMath::lerp(const float a, const float b, float t)
{
	t = clamp(t, 0.0f, 1.0f);

	return a * t + (1.0 - t) * b;
}

float SMath::smoothstep(const float a, const float b, float t)
{
	float tempT = (t - a) / (b - a);
	tempT = clamp(tempT, 0.0f, 1.0f);

	return tempT * tempT *(3.0 - 2.0 * tempT);
}

float SMath::fract(float num)
{
	return num - ((int)num);
}

float SMath::hashNoise(float x, float y)
{
	float numDecimals = 1000.0;
	x = round(x * numDecimals) / numDecimals;
	y = round(y * numDecimals) / numDecimals;

	double finalHashNoise;
	double intPart;

	finalHashNoise = modf(
		sin(x * 3541.234 + y * 7323.631) * 1374.424,
		&intPart
	);

	return finalHashNoise;
}

float SMath::perlinNoiseLayer(const float x, const float y)
{
	const float gridSize = 10.0;

	float st_x = smoothstep(0.0, 1.0, fract(x * gridSize));
	float st_y = smoothstep(0.0, 1.0, fract(y * gridSize));
	float id_x = floor(x * gridSize);
	float id_y = floor(y * gridSize);

	float upperLeft =	hashNoise(id_x + 0.0, id_y + 0.0);
	float upperRight =	hashNoise(id_x + 1.0, id_y + 0.0);
	float lowerLeft =	hashNoise(id_x + 0.0, id_y + 1.0);
	float lowerRight =	hashNoise(id_x + 1.0, id_y + 1.0);

	float upperMix = lerp(upperLeft, upperRight, st_x);
	float lowerMix = lerp(lowerLeft, lowerRight, st_x);

	float finalMix = lerp(upperMix, lowerMix, st_y);

	return finalMix;
}

float SMath::min(const float _val1, const float _val2)
{
	return _val1 < _val2 ? _val1 : _val2;
}

float SMath::max(const float _val1, const float _val2)
{
	return _val1 > _val2 ? _val1 : _val2;
}

// Returns the value clamped within an interwall.
float SMath::clamp(const float _val, const float _min, const float _max)
{
	if (_val < _min)
		return _min;
	else if (_val > _max)
		return _max;

	return _val;
}

// ResultVector becomes the cross product of vectorA and vectorB
void SMath::vectorCross(const sf::Vector3f& vectorA, const sf::Vector3f& vectorB, sf::Vector3f& resultVector)
{
	resultVector.x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
	resultVector.y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
	resultVector.z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;
}

// Normalize vector into a unit vector
void SMath::vectorNormalize(sf::Vector3f& vector)
{
	float length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

	vector /= length;
}

// Perlin noise
float SMath::perlinNoise(const float x, const float y)
{
	float currentNoise = 0.0f;

	currentNoise += perlinNoiseLayer(x * 4, y * 4) / 2;
	currentNoise += perlinNoiseLayer(x * 8, y * 8) / 4;
	currentNoise += perlinNoiseLayer(x * 16, y * 16) / 8;
	currentNoise += perlinNoiseLayer(x * 32, y * 32) / 16;
	currentNoise += perlinNoiseLayer(x * 64, y * 64) / 32;

	return currentNoise;
}