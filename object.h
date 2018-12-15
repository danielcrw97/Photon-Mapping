/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

 // Object is the base class for objects.

#pragma once

#include "ray.h"
#include "hit.h"
#include "material.h"

class Bounds;

class Object {
public:
	// Add bounds to object for quicker intersection testing for polygons
	Bounds* bounds;

	Material* material;

	Object()
	{

	}

	virtual void intersection(Ray ray, Hit &hit)
	{

	}
};
