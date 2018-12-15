#pragma once
#include "light.h"
#include "Plane.h"

const int SHADOW_SAMPLE_GRID_SIZE = 8;

class SquareLight : public Light
{
private:
	bool check_for_shadow(Ray shadowRay, float distanceToLight);
public:
	Plane plane;
	Vector normal;
	Vertex focalPoint;
	Bounds bounds;

	SquareLight(Plane plane, Vector normal, Vertex focalPoint, Colour col, Bounds bounds);

	float get_direction(Vertex &surface, Vector &dir);
	void get_intensity(Vertex &surface, Colour &intensity);
	void emit_photons(std::vector<std::shared_ptr<Photon>>& photons, int photonCount);
	void emit_caustic_photons(std::vector<std::shared_ptr<Photon>>& photons, int photonCount);
	void intersects(Ray ray, Hit& hit);
};

