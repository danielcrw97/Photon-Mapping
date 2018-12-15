/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// PolyMesh is a child class of Object and reads a simplified version of
// the ply format. It only supports triangle meshes.

#pragma once

#include "vertex.h"
#include "object.h"
#include "transform.h"

typedef int TriangleIndex[3];

class PolyMesh : public Object {
public:
	int vertex_count;
	int triangle_count;
    Vertex *vertex;
	TriangleIndex *triangle;

	PolyMesh(char *file, Transform &transform);
	void intersection(Ray ray, Hit &hit);
	void triangle_intersection(Ray ray, Hit &hit, int which_triangle);
	float test_edge(Vector &normal, Vertex &p, Vertex &v1, Vertex &v0);
};
