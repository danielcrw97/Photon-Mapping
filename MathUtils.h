#pragma once

#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif

#include "vector.h"

struct MathConstants
{
	static float const PI;
};


float getRandom0To1();

// Return a randomly cosined weighted vector in the same hemisphere as the normal.
// I.e. the vector returned is more likely to be in line with the normal.
Vector getCosineWeightedVector(Vector const& normal);