
#include "MathUtils.h"
#include "vector.h"
#include <cmath>
#include <cstdlib>


float const MathConstants::PI = ((float)M_PI);

float getRandom0To1()
{
	return (float)rand() / (float)RAND_MAX;
}

// Return a randomly cosined weighted vector in the same hemisphere as the normal.
// I.e. the vector returned is more likely to be in line with the normal.
Vector getCosineWeightedVector(Vector const& normal)
{
	Vector tang1;
	Vector tang2;

	Vector c1;
	Vector c2;
	normal.cross(Vector(0.0f, 0.0f, 1.0f), c1);
	normal.cross(Vector(0.0, 1.0, 0.0), c2);

	if (c1.magnitude() > c2.magnitude())
	{
		tang1 = c1;
	}
	else
	{
		tang1 = c2;
	}

	tang1.normalise();

	normal.cross(tang1, tang2);
	tang2.normalise();

	float sin_theta = sqrtf(getRandom0To1());
	float cos_theta = sqrtf(1 - (sin_theta * sin_theta));

	float psi = getRandom0To1() * 2.0f * (MathConstants::PI);

	// Three vector components
	float a = sin_theta * cosf(psi);
	float b = sin_theta * sinf(psi);
	float c = cos_theta;

	Vector v1(a * tang1.x, a * tang1.y, a * tang1.z);
	Vector v2(b * tang2.x, b * tang2.y, b * tang2.z);
	Vector v3(c * normal.x, c * normal.y, c * normal.z);

	Vector dir(v1.x + v2.x + v3.x, v1.y + v2.y + v3.y, v1.z + v2.z + v3.z);
	dir.normalise();
	return dir;
}