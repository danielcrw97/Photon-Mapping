#pragma once
#include "object.h"
#include "Triangle.h"
#include "transform.h"

class Plane : public Object
{
private:
	Triangle triangle1;
	Triangle triangle2;

public:
	Plane(Triangle triangle1, Triangle triangle2);
	void intersection(Ray ray, Hit& hit);
	void transform(Transform transform);
	~Plane();
};

