#include "SMath.h"

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
