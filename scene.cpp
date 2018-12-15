/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

#include "scene.h"
#include <iostream>
#include "MathUtils.h"
#include "RenderingConstants.h"

using namespace std;

Scene::Scene()
{
	object_list = vector<shared_ptr<Object>>();
	light_list = vector<shared_ptr<Light>>();
	globalPhotonMap = nullptr;
	causticPhotonMap = nullptr;
}

void Scene::trace(Ray ray, vector<shared_ptr<Object>> objects, Hit &hit)
{
	Hit current_hit;

	hit.flag = false;
	hit.t = 0.0f;
	hit.what = 0;

	for(shared_ptr<Object> object : objects)
	{
		Hit hit_current;

		object->intersection(ray, hit_current);

		if (hit_current.flag == true)
		{
			if (hit.flag == false)
			{
				hit = hit_current;

			}
			else if (hit_current.t < hit.t)
			{
				hit = hit_current;
			}
		}
	}
}

bool Scene::shadowtrace(Ray ray, vector<shared_ptr<Object>> objects, float distanceToLight)
{
	for (shared_ptr<Object> object : objects)
	{
		Hit hit_current;

		object->intersection(ray, hit_current);

		if (hit_current.flag == true)
		{
			if (hit_current.t < distanceToLight)
			{
				return true;
			}
		}
	}
	return false;
}

void Scene::raytrace(Ray ray, int level, vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, Colour &colour)
{
	// check we've not recursed too far.
	if (level > MAX_RAY_RECURSION)
	{
		return;
	}

	// first step, find the closest primitive

	Hit best_hit;
	trace(ray, objects, best_hit);

	// Check for light intersections.
	float minT = best_hit.t;
	shared_ptr<Light> intersectedLight = nullptr;
	for (shared_ptr<Light> light : lights)
	{
		Hit lightHit;
		light->intersects(ray, lightHit);
		if (lightHit.flag)
		{
			if (lightHit.t < minT)
			{
				minT = lightHit.t;
				intersectedLight = light;
			}
		}
	}
	if (intersectedLight != nullptr)
	{
		colour = intersectedLight->col;
		return;
	}

	// if we found a primitive then compute the colour we should see
	if (best_hit.flag)
	{
		
		// 1. Compute direct lighting for lit surface.
		for(shared_ptr<Light> light : lights)
		{
			Vector ldir;
			float litRatio = light->get_direction(best_hit.position, ldir);

			// Only bother computing colour if it is lit enough to care about.
			if (litRatio > 0.01f)
			{
				Colour intensity;
				Colour scaling;

				light->get_intensity(best_hit.position, scaling);

				Material* mat = best_hit.what->material;
				if (mat->tag == MaterialTag::LAMBERTIAN)
				{
					Vector tolight;
					tolight = ldir;
					tolight.negate();
					float diff = best_hit.normal.dot(tolight);
					if (diff > 0.0f) 
					{
						colour = mat->colour;
						colour.scaleColour(diff);
					}
				}

				intensity.scale(scaling);

				colour.add(intensity);

				colour.scaleColour(litRatio);
			}
		}

		
		// 2. Compute specular reflection if material supports it.
		if ((best_hit.what->material->tag == MaterialTag::SPECULAR))
		{		
			Ray reflectedRay;
			createReflectionRay(best_hit.normal, ray.direction, best_hit.position, reflectedRay);
			raytrace(reflectedRay, ++level, object_list, light_list, colour);
			colour.scale(best_hit.what->material->colour);
		}
		else if (best_hit.what->material->tag == MaterialTag::GLASS)
		{
			Vector direction = ray.direction;
			Vector n = best_hit.normal;
			Vector nl = n;
			if (n.dot(direction) > 0)
			{
				nl.scale(-1.0f);
			}

			// Create reflection vector.
			Ray reflectedRay;
			createReflectionRay(best_hit.normal, ray.direction, best_hit.position, reflectedRay);

			bool into = n.dot(nl) > 0;
			float nc = 1;
			float nt = 2.40f;
			float nnt = into ? nc / nt : nt / nc;
			float ddn = direction.dot(nl);
			float cos2theta = 1 - (nnt * nnt * (1 - ddn * ddn));

			//Total internal reflection
			if (cos2theta < 0) {
				raytrace(reflectedRay, ++level, object_list, light_list, colour);
			}

			Vector refractionDir = direction;
			refractionDir.scale(nnt);
			float refraction = (into ? 1 : -1) * ((ddn * nnt) + sqrtf(cos2theta));
			Vector n2 = n;
			n2.scale(refraction);
			refractionDir = refractionDir - n2;
			refractionDir.normalise();

			// Apply Schlicks approximation to get a reflection coefficient to find out how much light to reflect in a specular fashion.
			float a = nt - nc, b = nt + nc, R0 = a * a / (b*b), c = 1 - (into ? -ddn : refractionDir.dot(n));
			float reflectionCoefficient = R0 + (1 - R0)*c*c*c*c*c;

			raytrace(reflectedRay, ++level, object_list, light_list, colour);

			// UNCOMMENT THIS TO USE THE FRESNEL COEFFICIENT
			// The bunny does not seem to reflect much light when using Fresenl, which often results in a very dark bunny.
			//colour.scaleColour(reflectionCoefficient);

			colour.scale(best_hit.what->material->colour);
		}
		
		
		// 3. Compute caustics from the caustic photon map.
		if (causticPhotonMap != nullptr)
		{
			if (best_hit.what->material->tag == MaterialTag::LAMBERTIAN)
			{
				// Use filtering in the irradiance estimate.
				Colour irradiance = causticPhotonMap->irradianceEstimate(best_hit, 100, true);
				colour.add(irradiance);
			}
		}
		
		// 4. Compute global lighting from the global photon map.
		if (globalPhotonMap != nullptr)
		{
			if (best_hit.what->material->tag == MaterialTag::LAMBERTIAN)
			{
				// Do not use filtering in the irradiance estimate.
				Colour irradiance = globalPhotonMap->irradianceEstimate(best_hit, 300, false);
				colour.add(irradiance);
			}
		}
		
	}
	else
	{
		colour.r = 0.0f;
		colour.g = 0.0f;
		colour.b = 0.0f;
	}
}

void Scene::photontrace(Ray ray, int level, vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, Colour const& power, bool causticpath, vector<shared_ptr<Photon>>& photons)
{
	// check we've not recursed too far.
	if(level > MAX_RAY_RECURSION)
	{
		return;
	}

	Hit best_hit;
	trace(ray, objects, best_hit);

	// If we hit an object decide what to do with the photon
	if (best_hit.flag)
	{
		Material* mat = best_hit.what->material;
		Colour photonPow = power;
		photonPow.scale(best_hit.what->material->colour);

		if (mat->tag == MaterialTag::SPECULAR)
		{
			// Will eventually end up as a caustic light - so return.
			if (level == 1)
			{
				causticpath = true;
			}

			// Reflect the photon in the ideal mirror direction
			Ray reflectedRay;
			createReflectionRay(best_hit.normal, ray.direction, best_hit.position, reflectedRay);
			photontrace(reflectedRay, ++level, object_list, light_list, photonPow, causticpath, photons);
		}
		else if(mat->tag == MaterialTag::LAMBERTIAN)
		{
			// We either absorb or diffusely reflect the photon using russian roulette to decide.
			if (getRandom0To1() > mat->diffuseCoefficient)
			{
				// No direct lighting or caustics!
				if (level == 1 || causticpath)
				{
					return;
				}

				// Absorb the photon.
				photonPow.scaleColour(1.0f / (1 - mat->diffuseCoefficient));
				shared_ptr<Photon> photon = make_shared<Photon>(best_hit.position, photonPow, ray.direction);
				photons.push_back(photon);
			}
			else
			{
				//Diffusely reflect the photon in cosine weighted random direction above hemisphere.
				Vector photonDir = getCosineWeightedVector(best_hit.normal);
				float offset = 0.01f;
				Vertex offsetOrigin(best_hit.position.x + (offset * photonDir.x),
					best_hit.position.y + (offset * photonDir.y),
					best_hit.position.z + (offset * photonDir.z));
				Ray reflectedPhotonRay(offsetOrigin, photonDir);
				photonPow.scaleColour(1.0f / (mat->diffuseCoefficient));
				photontrace(reflectedPhotonRay, ++level, object_list, light_list, photonPow, false, photons);
			}
		}
		else if (best_hit.what->material->tag == MaterialTag::GLASS)
		{
			// If this is the first level the light is following a caustic path.
			if (level == 1)
			{
				causticpath = true;
			}

			Vector direction = ray.direction;
			Vector n = best_hit.normal;
			Vector nl = n;
			if (n.dot(direction) > 0)
			{
				nl.scale(-1.0f);
			}

			// Create reflection vector.
			Ray reflectedRay;
			createReflectionRay(best_hit.normal, ray.direction, best_hit.position, reflectedRay);

			// Apply fresnel equation
			bool into = n.dot(nl) > 0;
			float nc = 1; 
			float nt = RenderingConstants::GLASS_REFRACTION_INDEX;
			float nnt = into ? nc / nt : nt / nc; 
			float ddn = direction.dot(nl); 
			float cos2theta = 1 - (nnt * nnt * (1 - ddn * ddn));

			//Total internal reflection
			if (cos2theta < 0) {
				photontrace(reflectedRay, ++level, object_list, light_list, photonPow, causticpath, photons);
			}

			Vector refractionDir = direction;
			refractionDir.scale(nnt);
			float refraction = (into ? 1 : -1) * ((ddn * nnt) + sqrtf(cos2theta));
			Vector n2 = n;
			n2.scale(refraction);
			refractionDir = refractionDir - n2;
			refractionDir.normalise();
			
			float offset = 0.01f;
			Vertex offsetRefractionOrigin(best_hit.position.x + (offset * refractionDir.x),
									best_hit.position.y + (offset * refractionDir.y),
									best_hit.position.z + (offset * refractionDir.z));
			Ray refractedRay(offsetRefractionOrigin, refractionDir);

			float a = nt - nc, b = nt + nc, R0 = a * a / (b*b), c = 1 - (into ? -ddn : refractionDir.dot(n));
			float reflectionCoefficient = R0 + (1 - R0)*c*c*c*c*c; 
			float refractionCoefficient = 1 - reflectionCoefficient;
			float prob = .25f + .5f*reflectionCoefficient; 
			float reflectionScalar = reflectionCoefficient / prob; 
			float refractionScalar = refractionCoefficient / (1 - prob);
			if (getRandom0To1() < prob)
			{
				photonPow.scaleColour(reflectionScalar);
				photontrace(reflectedRay, ++level, object_list, light_list, photonPow, causticpath, photons);
			}
			else
			{
				// Photon trace the refacted ray.
				photonPow.scaleColour(refractionScalar);
				photontrace(refractedRay, ++level, object_list, light_list, photonPow, causticpath, photons);
			}
		}
	}
}

void Scene::causticsphotontrace(Ray ray, int level, vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, Colour const & power, vector<shared_ptr<Photon>>& photons)
{
	// check we've not recursed too far.
	if (level > MAX_RAY_RECURSION)
	{
		return;
	}

	Hit best_hit;
	trace(ray, objects, best_hit);

	// If we hit an object store the photon.
	if (best_hit.flag)
	{
		Material* mat = best_hit.what->material;
		Colour photonPow = power;
		photonPow.scale(best_hit.what->material->colour);

		// Caustics are LS+DE light paths - so if we hit a diffuse surface first just return and try again!
		// Could potentially be optimised by projection mapping - does not save much time overall though.
		if ((level == 1) && mat->tag == MaterialTag::LAMBERTIAN)
		{
			return;
		}

		if (mat->tag == MaterialTag::SPECULAR)
		{
			Ray reflectedRay;
			createReflectionRay(best_hit.normal, ray.direction, best_hit.position, reflectedRay);
			causticsphotontrace(reflectedRay, ++level, object_list, light_list, photonPow, photons);
		}
		else if (mat->tag == MaterialTag::LAMBERTIAN)
		{
			// End the light path and store a photon on hitting a diffuse surface!
			shared_ptr<Photon> photon = make_shared<Photon>(best_hit.position, photonPow, ray.direction);
			photons.push_back(photon);
		}
		else if (best_hit.what->material->tag == MaterialTag::GLASS)
		{
			Vector direction = ray.direction;
			Vector n = best_hit.normal;
			Vector nl = n;
			if (n.dot(direction) > 0)
			{
				nl.scale(-1.0f);
			}

			// Create reflection vector.
			Ray reflectedRay;
			createReflectionRay(best_hit.normal, ray.direction, best_hit.position, reflectedRay);

			bool into = n.dot(nl) > 0;
			float nc = 1;
			float nt = RenderingConstants::GLASS_REFRACTION_INDEX;
			float nnt = into ? nc / nt : nt / nc;
			float ddn = direction.dot(nl);
			float cos2theta = 1 - (nnt * nnt * (1 - ddn * ddn));

			//Total internal reflection
			if (cos2theta < 0) {
				causticsphotontrace(reflectedRay, ++level, object_list, light_list, photonPow, photons);
			}

			Vector refractionDir = direction;
			refractionDir.scale(nnt);
			float refraction = (into ? 1 : -1) * ((ddn * nnt) + sqrtf(cos2theta));
			Vector n2 = n;
			n2.scale(refraction);
			refractionDir = refractionDir - n2;
			refractionDir.normalise();

			float offset = 0.01f;
			Vertex offsetRefractionOrigin(best_hit.position.x + (offset * refractionDir.x),
				best_hit.position.y + (offset * refractionDir.y),
				best_hit.position.z + (offset * refractionDir.z));
			Ray refractedRay(offsetRefractionOrigin, refractionDir);

			// Apply fresnel equation
			float a = nt - nc, b = nt + nc, R0 = a * a / (b*b), c = 1 - (into ? -ddn : refractionDir.dot(n));
			float reflectionCoefficient = R0 + (1 - R0)*c*c*c*c*c; 
			float refractionCoefficient = 1 - reflectionCoefficient;
			float P = .25f + .5f*reflectionCoefficient, RP = reflectionCoefficient / P, TP = refractionCoefficient / (1 - P);
			if (getRandom0To1() < P)
			{
				photonPow.scaleColour(RP);
				causticsphotontrace(reflectedRay, ++level, object_list, light_list, photonPow, photons);
			}
			else
			{
				// Photon trace the refacted ray.
				photonPow.scaleColour(TP);
				causticsphotontrace(refractedRay, ++level, object_list, light_list, photonPow, photons);
			}
		}
	}
}



shared_ptr<PhotonMap> Scene::buildCausticsPhotonMap(vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, int photonCount)
{
	std::cout << "Sending caustic photons into scene..." << std::endl;
	vector<shared_ptr<Photon>> photons;

	for (shared_ptr<Light> light : lights)
	{
		light->emit_caustic_photons(photons, photonCount);
	}

	std::cout << "Building caustics photon map..." << std::endl;
	return make_shared<PhotonMap>(photons);
}

shared_ptr<PhotonMap> Scene::buildGlobalPhotonMap(vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, int photonCount)
{
	std::cout << "Sending global photons into scene..." << std::endl;
	vector<shared_ptr<Photon>> photons;

	for (shared_ptr<Light> light : lights)
	{
		light->emit_photons(photons, photonCount);
	}

	std::cout << "Building global photon map..." << std::endl;
	return make_shared<PhotonMap>(photons);
}

void Scene::createReflectionRay(Vector const & normal, Vector const & dir, Vertex const& pos, Ray & reflectionRay)
{
	Vector reflectedDir;
	normal.reflection(dir, reflectedDir);
	float offset = 0.001f;
	Vertex offsetOrigin(pos.x + (offset * reflectedDir.x),
				pos.y + (offset * reflectedDir.y),
				pos.z + (offset * reflectedDir.z));
	reflectionRay = Ray(offsetOrigin, reflectedDir);
}

void Scene::addObject(shared_ptr<Object> object)
{
	object_list.push_back(object);
}

void Scene::addLight(shared_ptr<Light> light)
{
	light_list.push_back(light);
}
