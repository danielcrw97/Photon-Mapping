/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Vector is a class to store and maniplulate 3D vectors.

#pragma once

#include <cmath>

class Vector {
public:
	float x;
	float y;
	float z;

	Vector(float px, float py, float pz)
	{
		x = px;
		y = py;
		z = pz;
	}

	Vector()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	void scale(float scale)
	{
		x = x * scale;
		y = y * scale;
		z = z * scale;
	}

	void normalise()
	{
		float len = (float)sqrt((double)(x*x + y*y + z*z));
		x = x / len;
		y = y / len;
		z = z / len;
	}

	float dot(Vector &other) const
	{
		return x*other.x + y*other.y + z*other.z;
	}


	void reflection(Vector initial, Vector &reflect) const
	{
		float d;

		d = dot(initial);
		d = d * 2.0f;

		reflect.x = initial.x - d * x;
		reflect.y = initial.y - d * y;
		reflect.z = initial.z - d * z;
	}

	void negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	Vector operator-(Vector const& right) const
	{
		return Vector(x - right.x, y - right.y, z - right.z);
	}

	void cross(Vector const& other, Vector &result) const
	{
	  result.x = y*other.z - z*other.y;
	  result.y = z*other.x - x*other.z;
	  result.z = x*other.y - y*other.x;
	}

	float magnitude() const
	{
		return sqrtf((x*x) + (y*y) + (z*z));
	}
};
