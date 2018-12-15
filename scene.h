/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

#pragma once

#include "colour.h"
#include "ray.h"
#include "object.h"
#include "light.h"
#include "hit.h"
#include "PhotonMap.h"

 // Scene is a class that is used to build a scene database of objects
 // and lights and then trace a ray through it.
using namespace std;
class Scene {
public:
	const int MAX_RAY_RECURSION = 8;

	vector<shared_ptr<Object>> object_list;
	vector<shared_ptr<Light>> light_list;
	std::shared_ptr<PhotonMap> globalPhotonMap;
	std::shared_ptr<PhotonMap> causticPhotonMap;

	Scene();

	// Trace a ray through the scene and find the closest if any object
	// intersection in front of the ray.
	void trace(Ray ray, vector<shared_ptr<Object>> objects, Hit &hit);

	// A quicker version of tracing for shadows that returns the moment that
	// returns true when the ray hits an object closer than the light.
	bool shadowtrace(Ray ray, vector<shared_ptr<Object>> objects, float distanceToLight);

	// Trace a ray through the scene and return its colour. This function
	// is the one that should recurse down the reflection/refraction tree.
	void raytrace(Ray ray, int level, vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, Colour &colour);

	// Traces photons through the scene, eventually storing photons that are absorbed by a diffuse surface into the vector of photons.
	void photontrace(Ray ray, int level, vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, Colour const& power, bool causticpath, vector<shared_ptr<Photon>>& photons);

	// Traces photons through the scene following a LS+DE light path.
	void causticsphotontrace(Ray ray, int level, vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, Colour const& power, vector<shared_ptr<Photon>>& photons);

	// Build a Photon Map specifically for caustics (i.e. specular to diffuse reflection).
	std::shared_ptr<PhotonMap> buildCausticsPhotonMap(vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, int photonCount);

	// Build a Photon Map by sending photons from all the lights in the scene 
	// and then storing them inside a balanced KD tree. 
	std::shared_ptr<PhotonMap> buildGlobalPhotonMap(vector<shared_ptr<Object>> objects, vector<shared_ptr<Light>> lights, int photonCount);

	// Create a reflection ray with a slightly offset origin to avoid intersection issues.
	void createReflectionRay(Vector const& normal, Vector const& dir, Vertex const& pos, Ray& reflectionRay);
	
	void addObject(std::shared_ptr<Object> object);
	void addLight(std::shared_ptr<Light> light);
};