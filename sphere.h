/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Sphere is a child class of Object and supports simple spheres defined with
// a center and radius.

#pragma once

#include "vertex.h"
#include "object.h"

class Sphere : public Object {
public:

	Vertex center;
	float  radius;

	Sphere(Vertex c, float r);
	void intersection(Ray ray, Hit &hit);
};
