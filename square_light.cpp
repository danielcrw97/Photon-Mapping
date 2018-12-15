#include "square_light.h"
#include "scene.h"
#include <random>
#include "Bounds.h"
#include "MathUtils.h"
#include "RenderingConstants.h"

SquareLight::SquareLight(Plane plane, Vector normal, Vertex focalPoint, Colour col, Bounds bounds)
	: plane(plane), normal(normal), focalPoint(focalPoint), bounds(bounds)
{
	this->col = col;
	normal.normalise();
}

float SquareLight::get_direction(Vertex& surface, Vector& dir)
{
	// Check the light is in the correct hemisphere
	dir = surface - focalPoint;
	dir.normalise();
	float theta = acosf(dir.dot(normal));
	float halfPI = MathConstants::PI / 2.0f;
	if (theta > halfPI)
	{
		return 0.0f;
	}

	// If not using shadow sampling cast one shadow ray directly to the focal point of the light
	if (!RenderingConstants::USE_SHADOW_SAMPLING)
	{
		Vector rayDir = focalPoint - surface;
		rayDir.normalise();
		float offset = 0.3f;
		Vertex offsetShadowOrigin(surface.x + (rayDir.x * offset),
			surface.y + (rayDir.y * offset),
			surface.z + (rayDir.z * offset));
		float distanceBetweenLight = (focalPoint - offsetShadowOrigin).magnitude();
		Ray shadowRay(offsetShadowOrigin, rayDir);
		if (check_for_shadow(shadowRay, distanceBetweenLight))
		{
			return 0.0f;
		}
		else
		{
			return 1.0f;
		}
	}
	
	// Do shadow sampling to create soft area shadows.
	float shadowRatio = 1.0f;
	float xDif = bounds.difX / (float) RenderingConstants::SHADOW_SAMPLE_GRID_SIZE;
	float zDif = bounds.difZ / (float)RenderingConstants::SHADOW_SAMPLE_GRID_SIZE;
	int const numberOfRays = RenderingConstants::SHADOW_SAMPLE_GRID_SIZE * RenderingConstants::SHADOW_SAMPLE_GRID_SIZE;
	for (int i = 0; i < RenderingConstants::SHADOW_SAMPLE_GRID_SIZE; ++i)
	{
		for (int j = 0; j < RenderingConstants::SHADOW_SAMPLE_GRID_SIZE; ++j)
		{
			float rand1 = getRandom0To1();
			float rand2 = getRandom0To1();

			float randX = xDif * rand1;
			float randZ = zDif * rand2;

			float xPos = bounds.minX + (xDif * i) + randX;
			float zPos = bounds.minZ + (zDif * j) + randZ;


			// Leave y out of the equation as it should be constant for a ceiling light.
			Vertex lightPoint(xPos, bounds.minY, zPos);

			Vector rayDir = lightPoint - surface;
			rayDir.normalise();
			float offset = 0.05f;
			Vertex offsetShadowOrigin(surface.x + (rayDir.x * offset),
				surface.y + (rayDir.y * offset),
				surface.z + (rayDir.z * offset));
			float distanceBetweenLight = (lightPoint - offsetShadowOrigin).magnitude();

			Ray sampleShadowRay(offsetShadowOrigin, rayDir);

			if (check_for_shadow(sampleShadowRay, distanceBetweenLight))
			{
				shadowRatio = shadowRatio - (1 / (float) numberOfRays);
			}
		}
	}
	return shadowRatio;
}

void SquareLight::get_intensity(Vertex& surface, Colour& intensity)
{
	intensity = col;
}

// Generating photons from a square light includes a random element
// The light is more likely to generate photons inline with the normal
// to the plane (i.e. (0, -1, 0)). 
void SquareLight::emit_photons(std::vector<std::shared_ptr<Photon>>& photons, int photonCount)
{
	Colour photonPower = col;
	int count = 0;
	photonPower.scaleColour(RenderingConstants::GLOBAL_PHOTON_POWER / (float)photonCount);
	while (count < photonCount)
	{
		Vector photonDir = getCosineWeightedVector(normal);
		Ray photonRay(focalPoint, photonDir);
		scene->photontrace(photonRay, 1, scene->object_list, scene->light_list, photonPower, false, photons);
		++count;
	}
}

void SquareLight::emit_caustic_photons(std::vector<std::shared_ptr<Photon>>& photons, int photonCount)
{
	Colour photonPower = col;
	photonPower.scaleColour(RenderingConstants::CAUSTIC_PHOTON_POWER / (float)photonCount);

	// Unlike emitting global photons, we wait until all the photons have hit a specular surface.
	// A possible optimisation here would be to use projection mapping towards specular surfaces...
	// But in such a dense scene there wouldn't be that much in terms of performance gain
	// Instead we will use rejection sampling (wait till 10000 photons have hit a specuclar surface)
	while ((int)photons.size() < photonCount)
	{
		Vector photonDir = getCosineWeightedVector(normal);
		Ray photonRay(focalPoint, photonDir);
		scene->causticsphotontrace(photonRay, 1, scene->object_list, scene->light_list, photonPower, photons);
	}
}

void SquareLight::intersects(Ray ray, Hit & hit)
{
	plane.intersection(ray, hit);
}

bool SquareLight::check_for_shadow(Ray shadowRay, float distanceToLight)
{
	return scene->shadowtrace(shadowRay, scene->object_list, distanceToLight);
}
