#pragma once
#include "vertex.h"
#include "hit.h"
#include "ray.h"

class Triangle
{
public:
	Vertex v1;
	Vertex v2;
	Vertex v3;

	Vector edge1;
	Vector edge2;
	Vector normal;

	Triangle(Vertex v1, Vertex v2, Vertex v3)
		: v1(v1), v2(v2), v3(v3)
	{
		edge1 = v2 - v1;
		edge2 = v3 - v1;
		edge1.cross(edge2, normal);
		normal.normalise();
	}

	// Used the Moller-Trumbore intersection algorithm to test triangle intersection
	Hit intersects(Ray& ray)
	{
		const float EPSILON = 0.0000001f;

		Hit hit;
		hit.flag = false;

		Vector h;
		ray.direction.cross(edge2, h);
		float a = edge1.dot(h);
		if (a > -EPSILON && a < EPSILON)
			return hit;   

		float f = 1.0f / a;
		Vector s = Vector(ray.position.x - v1.x, ray.position.y - v1.y, ray.position.z - v1.z);
		float u = f * (s.dot(h));

		if (u < 0.0f || u > 1.0f)
			return hit;

		Vector q;
		s.cross(edge1, q);
		float v = f * ray.direction.dot(q);
		if (v < 0.0f || u + v > 1.0f)
			return hit;

		float t = f * edge2.dot(q);
		if (t > 0.0f) 
		{
			hit.t = t;
			hit.position = Vertex(ray.position.x + (ray.direction.x * t),
				ray.position.y + (ray.direction.y * t),
				ray.position.z + (ray.direction.z * t));
			hit.flag = true;
			hit.normal = normal;
			if (hit.normal.dot(ray.direction) > 0.0f)
			{
				hit.normal.negate();
			}
			return hit;
		}
		return hit;
	}
};


