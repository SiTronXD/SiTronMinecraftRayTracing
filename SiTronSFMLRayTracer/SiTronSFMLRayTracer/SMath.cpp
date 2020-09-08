#include "SMath.h"

float SMath::Clamp(const float _val, const float _min, const float _max)
{
	if (_val < _min)
		return _min;
	else if (_val > _max)
		return _max;

	return _val;
}
