#include "Plane.h"

Plane::Plane(Triangle triangle1, Triangle triangle2)
	: Object(), triangle1(triangle1), triangle2(triangle2)
{
}

void Plane::intersection(Ray ray, Hit& hit) 
{
	hit = triangle1.intersects(ray);
	if (hit.flag)
	{
		hit.what = this;
		return;
	}
	hit = triangle2.intersects(ray);
	if (hit.flag)
	{
		hit.what = this;
	}
}

void Plane::transform(Transform transform)
{
	transform.apply(triangle1.v1);
	transform.apply(triangle1.v2);
	transform.apply(triangle1.v3);
	transform.apply(triangle2.v1);
	transform.apply(triangle2.v2);
	transform.apply(triangle2.v3);

	triangle1.edge1 = triangle1.v2 - triangle1.v1;
	triangle1.edge2 = triangle1.v3 - triangle1.v1;
	triangle2.edge1 = triangle2.v2 - triangle2.v1;
	triangle2.edge2 = triangle2.v3 - triangle2.v1;
}


Plane::~Plane()
{
}
