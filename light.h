/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Light is the base class for lights.

#pragma once

#include "vertex.h"
#include "vector.h"
#include "colour.h"
#include "ray.h"
#include "hit.h"
#include "Bounds.h"
#include <memory>
#include <vector>

class Photon;

class Light {
public:
	class Scene *scene;
	Colour col;

	Light()
	{
	}

	// Get the direction towards the light at the point on the surface as well as shadow sampling.
	// Returns a float indicating what the intensity of the light should be 
	// due to shadows (0, means no shadows, 1 means no light at all).
	virtual float get_direction(Vertex &surface, Vector &dir)
	{
		return false;
	}

	// Get the intensity of the light in the direction of the surface
	virtual void get_intensity(Vertex& surface, Colour &intensity)
	{

	}

	// Adds photons to our list of photons depending on how the light distributes them.
	virtual void emit_photons(std::vector<std::shared_ptr<Photon>>& photons, int photonCount)
	{

	}

	// Add caustic photons to our list of photons. Only photons that follow LS+DE will be stored here.
	virtual void emit_caustic_photons(std::vector<std::shared_ptr<Photon>>& photons, int photonCount)
	{

	}

	// For light with geometry, we can now check for intersections.
	virtual void intersects(Ray ray, Hit& hit)
	{

	}
};
