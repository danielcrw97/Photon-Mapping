/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

#include "polymesh.h"
#include <cmath>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Bounds.h"
using namespace std;

PolyMesh::PolyMesh(char *file, Transform &transform)
{
	int count;
	ifstream meshfile(file);

	cerr << "Opening meshfile: " << file << endl;

	if (!meshfile.is_open())
	{
		cerr << "Problem reading meshfile (not found)." << endl;
		meshfile.close();
		exit(-1);
	}

	string line;

	try {
		getline(meshfile, line);
	}
	catch (ifstream::failure e)
	{
		cerr << "Problem reading meshfile (getline failed)." << endl;
	}

	if (line.compare("kcply") != 0)
	{
		cerr << "Problem reading meshfile (not kcply)." << endl;
		meshfile.close();
		exit(-1);
	}

	try {
		getline(meshfile, line);
	}
	catch (ifstream::failure e)
	{
		cerr << "Problem reading meshfile (getline failed)." << endl;
		exit(-1);
	}

	istringstream vertex_iss(line);
	string vertex_element;
	string vertex_label;

	vertex_iss >> vertex_element >> vertex_label >> vertex_count;

	if ((vertex_element.compare("element") != 0) || (vertex_label.compare("vertex") != 0))
	{
		cerr << "Problem reading meshfile (element vertex)." << endl;
		meshfile.close();
		exit(-1);
	}

	cerr << "Expect " << vertex_count << " vertices." << endl;

	try {
		getline(meshfile, line);
	}
	catch (ifstream::failure e)
	{
		cerr << "Problem reading meshfile (getline failed)." << endl;
		exit(-1);
	}

	istringstream triangle_iss(line);
	string triangle_element;
	string triangle_label;

	triangle_iss >> triangle_element >> triangle_label >> triangle_count;

	if ((triangle_element.compare("element") != 0) || (triangle_label.compare("face") != 0))
	{
		cerr << "Problem reading meshfile (element triangle)." << endl;
		meshfile.close();
		exit(-1);
	}

	cerr << "Expect " << triangle_count << " triangles." << endl;

	vertex = new Vertex[vertex_count];

	triangle = new TriangleIndex[triangle_count];

	float minX = 1000.0f;
	float maxX = -1000.0f;
	float minY = 1000.0f;
	float maxY = -1000.0f;
	float minZ = 1000.0f;
	float maxZ = -1000.0f;

	int i;

	for (i = 0; i < vertex_count; i += 1)
	{
		try {
			getline(meshfile, line);
		}
		catch (ifstream::failure e)
		{
			cerr << "Problem reading meshfile (getline failed)." << endl;
			exit(-1);
		}

		vertex_iss.clear();
		vertex_iss.str(line);

		vertex_iss >> vertex[i].x >> vertex[i].y >> vertex[i].z;

		transform.apply(vertex[i]);

		minX = vertex[i].x < minX ? vertex[i].x : minX;
		maxX = vertex[i].x > maxX ? vertex[i].x : maxX;
		minY = vertex[i].y < minY ? vertex[i].y : minY;
		maxY = vertex[i].y > maxY ? vertex[i].y : maxY;
		minZ = vertex[i].z < minZ ? vertex[i].z : minZ;
		maxZ = vertex[i].z > maxZ ? vertex[i].z : maxZ;
	}

	bounds = new Bounds(minX, maxX, minY, maxY, minZ, maxZ);

	for (i = 0; i < triangle_count; i += 1)
	{
		try {
			getline(meshfile, line);
		}
		catch (ifstream::failure e)
		{
			cerr << "Problem reading meshfile (getline failed)." << endl;
			exit(-1);
		}

		triangle_iss.clear();
		triangle_iss.str(line);

		triangle_iss >> count >> triangle[i][0] >> triangle[i][1] >> triangle[i][2];

		if (count != 3)
		{
			cerr << "Problem reading meshfile (non-triangle present)." << endl;
			exit(-1);
		}
	}

	meshfile.close();
	cerr << "Meshfile read." << endl;
}

float PolyMesh::test_edge(Vector &normal, Vertex &p, Vertex &v1, Vertex &v0)
{
	Vector edge;

	edge.x = v1.x - v0.x;
	edge.y = v1.y - v0.y;
	edge.z = v1.z - v0.z;

	Vector vp;

	vp.x = p.x - v0.x;
	vp.y = p.y - v0.y;
	vp.z = p.z - v0.z;

	Vector c;

	edge.cross(vp, c);

	return normal.dot(c);
}

void PolyMesh::triangle_intersection(Ray ray, Hit &hit, int which_triangle)
{
	Vector edge1, edge2;

	edge1.x = vertex[triangle[which_triangle][1]].x - vertex[triangle[which_triangle][0]].x;
	edge1.y = vertex[triangle[which_triangle][1]].y - vertex[triangle[which_triangle][0]].y;
	edge1.z = vertex[triangle[which_triangle][1]].z - vertex[triangle[which_triangle][0]].z;

	edge2.x = vertex[triangle[which_triangle][2]].x - vertex[triangle[which_triangle][0]].x;
	edge2.y = vertex[triangle[which_triangle][2]].y - vertex[triangle[which_triangle][0]].y;
	edge2.z = vertex[triangle[which_triangle][2]].z - vertex[triangle[which_triangle][0]].z;

	Vector normal;
	edge1.cross(edge2, normal);
	normal.normalise();

	Vector h;
	float const EPSILON = 0.000001f;
	ray.direction.cross(edge2, h);
	float a = edge1.dot(h);
	if (a > -EPSILON && a < EPSILON)
		return;

	hit.flag = false;

	Vertex v0 = vertex[triangle[which_triangle][0]];

	float f = 1.0f / a;
	Vector s = Vector(ray.position.x - v0.x, ray.position.y - v0.y, ray.position.z - v0.z);
	float u = f * (s.dot(h));

	if (u < 0.0f || u > 1.0f)
		return;

	Vector q;
	s.cross(edge1, q);
	float v = f * ray.direction.dot(q);
	if (v < 0.0f || u + v > 1.0f)
		return;

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
		hit.what = this;
		return;
	}
	return;
}

void PolyMesh::intersection(Ray ray, Hit &hit)
{
	
	if (!bounds->intersects(ray))
	{
		return;
	}

	Hit current_hit;
	int i;

	hit.flag = false;

	// Check each triangle any find closest if any intersecion

	for (i = 0; i < triangle_count; i += 1)
	{
		triangle_intersection(ray, current_hit, i);

		if (current_hit.flag != false)
		{
			if (hit.flag == false)
			{
				hit = current_hit;

			}
			else if (current_hit.t < hit.t)
			{
				hit = current_hit;
			}
		}
	}

	if (hit.flag == true)
	{
		if (hit.normal.dot(ray.direction) > 0.0)
		{
			hit.normal.negate();
		}
	}
}
