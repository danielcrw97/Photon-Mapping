/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Hit is a class to store and maniplulate information about an intersection
// between a ray and an object.

#pragma once

#include "vertex.h"
#include "Vector.h"

class Object;

class Hit {
public:
	bool flag;
	float t;
	Object *what;
	Vertex position;
	Vector normal;
};
