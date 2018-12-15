#pragma once
#include "vertex.h"
#include "sphere.h"
#include "transform.h"
#include "MathUtils.h"

// Inclusive bounds for a 3D object.

class Bounds
{
public:
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
	float difX;
	float difY;
	float difZ;

	Bounds(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
		:minX(minX), maxX(maxX), minY(minY), maxY(maxY), minZ(minZ), maxZ(maxZ)
	{
		difX = maxX - minX;
		difY = maxY - minY;
		difZ = maxZ - minZ;
	}

	Bounds(Vertex const& min, Vertex const& max)
	{
		minX = min.x;
		minY = min.y;
		minZ = min.z;
		maxX = max.x;
		maxY = max.y;
		maxZ = max.z;
		difX = maxX - minX;
		difY = maxY - minY;
		difZ = maxZ - minZ;
	}
	
	bool isInBounds(Vertex const& point)
	{
		if ((point.x < minX) || (point.x > maxX))
		{
			return false;
		}
		if ((point.y < minY) || (point.y > maxY))
		{
			return false;
		}
		if ((point.z < minZ) || (point.z > maxZ))
		{
			return false;
		}
		return true;
	}

	bool intersects(Ray const& ray)
	{
		// Create bounding sphere to check for intersection.
		Vertex midPoint(minX + (difX / 2.0f), minY + (difY / 2.0f), minZ + (difZ / 2.0f));

		float radius;
		if (difX > difY && difX > difZ)
		{
			radius = difX / 2.0f;
		}
		else if (difY > difZ)
		{
			radius = difY / 2.0f;
		}
		else
		{
			radius = difZ / 2.0f;
		}

		Sphere boundingSphere(midPoint, radius);
		Hit sphereHit;
		boundingSphere.intersection(ray, sphereHit);
		return sphereHit.flag;
	}

	void transform(Transform& transform)
	{
		Vertex min(minX, minY, minZ);
		Vertex max(maxX, maxY, maxZ);

		transform.apply(min);
		transform.apply(max);
		minX = min.x;
		minY = min.y;
		minZ = min.z;
		maxX = max.x;
		maxY = max.y;
		maxZ = max.z;
	}

	Vertex randomPointInBounds()
	{
		float rand1 = getRandom0To1();
		float rand2 = getRandom0To1();
		float rand3 = getRandom0To1();
		float x = (rand1 * difX) + minX;
		float y = (rand2 * difY) + minY;
		float z = (rand3 * difZ) + minZ;
		return Vertex(x, y, z);
	}
};