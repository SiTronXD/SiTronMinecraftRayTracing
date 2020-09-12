#include "Ray.h"

Ray::Ray(sf::Vector3f _position, sf::Vector3f _direction)
{
	position = _position;
	direction = _direction;

	oneOverDirection = sf::Vector3f(
		1.0f / direction.x,
		1.0f / direction.y,
		1.0f / direction.z
	);
}

Ray::~Ray()
{
}

Hit Ray::GetClosestBoxHit(sf::Vector3i boxPos)
{
	Hit newHitInfo;

	sf::Vector3f minCorner = (sf::Vector3f) boxPos - sf::Vector3f(0.5f, 0.5f, 0.5f);
	sf::Vector3f maxCorner = (sf::Vector3f) boxPos + sf::Vector3f(0.5f, 0.5f, 0.5f);

	float t1 = (minCorner.x - position.x) * oneOverDirection.x;
	float t2 = (maxCorner.x - position.x) * oneOverDirection.x;
	float t3 = (minCorner.y - position.y) * oneOverDirection.y;
	float t4 = (maxCorner.y - position.y) * oneOverDirection.y;
	float t5 = (minCorner.z - position.z) * oneOverDirection.z;
	float t6 = (maxCorner.z - position.z) * oneOverDirection.z;

	float tMin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tMax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// Box is behind ray, the ray doesn't intersect the box at all, 
	// or there is something closer to the ray
	if (tMax < 0.0 || tMin > tMax || tMin > hitInfo.t)
	{
		return newHitInfo;
	}

	// Ray hit box!
	float t = tMin;
	newHitInfo.t = t;
	newHitInfo.point = position + direction * t;

	// Horizontal sides
	if (t == t1 || t == t2)
	{
		newHitInfo.normal = sf::Vector3f(t == t1 ? -1.0 : 1.0, 0.0, 0.0);
	}
	// Top or bottom
	else if (t == t3 || t == t4)
	{
		newHitInfo.normal = sf::Vector3f(0.0f, t == t3 ? -1.0 : 1.0, 0.0);
	}
	// Other sides
	else if (t == t5 || t == t6)
	{
		newHitInfo.normal = sf::Vector3f(0.0f, 0.0, t == t5 ? -1.0 : 1.0);
	}

	return newHitInfo;
}
