#include "Hit.h"

Hit::Hit()
{
	set(
		std::numeric_limits<float>::max(), 
		sf::Vector3f(0.0f, 0.0f, 0.0f), 
		sf::Vector3f(0.0f, 1.0f, 0.0f)
	);
}

Hit::~Hit()
{
}

const void Hit::set(float _t, sf::Vector3f _hitPoint, sf::Vector3f _hitNormal)
{
	t = _t;
	point = _hitPoint;
	normal = _hitNormal;
}

bool Hit::hitSomething()
{
	return t < std::numeric_limits<float>::max();
}

Hit Hit::getClosestHit(Hit hit1, Hit hit2)
{
	if (hit1.t <= hit2.t)
		return hit1;
	
	return hit2;
}
