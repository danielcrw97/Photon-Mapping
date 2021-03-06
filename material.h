/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Material is the base class for materials.

#pragma once

#include "vector.h"
#include "colour.h"

enum class MaterialTag
{
	LAMBERTIAN,
	SPECULAR,
	GLASS
};

class Material {
public:
	MaterialTag tag;
	Colour colour;
	float refractionIndex;
	float diffuseCoefficient;
	float specularCoefficient;
	float absorptionCoefficient;

	virtual void compute_base_colour(Colour &result)
	{
		result.r = 0.0f;
		result.g = 0.0f;
		result.b = 0.0f;
	}

	virtual void compute_light_colour(Vector &viewer, Vector &normal, Vector &ldir, Colour &result)
	{
		result.r = 0.0f;
		result.g = 0.0f;
		result.b = 0.0f;
	}

	float getDiffuseCoefficient()
	{
		if (tag == MaterialTag::SPECULAR)
		{
			return 0.0f;
		}
		return diffuseCoefficient;
	}

	float getSpecularCoefficient()
	{
		if (tag == MaterialTag::LAMBERTIAN)
		{
			return 0.0f;
		}
		return specularCoefficient;
	}

	float getAbsorptionCoefficient()
	{
		return absorptionCoefficient;
	}
};
