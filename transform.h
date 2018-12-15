/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

 // Transform is a class to store, manipulate and apply transforms.

#pragma once

#include "vertex.h"
#include "Matrix.h"

class Transform {
public:

	Matrix matrix;

	Transform()
	{
		matrix = Matrix();
	};

	Transform(float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p)
	{
		this->matrix = Matrix(a, b, c, d,
			e, f, g, h,
			i, j, k, l,
			m, n, o, p);
	};

	void apply(Vertex &in, Vertex &out)
	{
		out.x = matrix[0][0] * in.x
			+ matrix[0][1] * in.y
			+ matrix[0][2] * in.z
			+ matrix[0][3] * in.w;
		out.y = matrix[1][0] * in.x
			+ matrix[1][1] * in.y
			+ matrix[1][2] * in.z
			+ matrix[1][3] * in.w;
		out.z = matrix[2][0] * in.x
			+ matrix[2][1] * in.y
			+ matrix[2][2] * in.z
			+ matrix[2][3] * in.w;
		out.w = matrix[3][0] * in.x
			+ matrix[3][1] * in.y
			+ matrix[3][2] * in.z
			+ matrix[3][3] * in.w;
	};

	void apply(Vertex &in)
	{
		float x, y, z, w;

		x = matrix[0][0] * in.x
			+ matrix[0][1] * in.y
			+ matrix[0][2] * in.z
			+ matrix[0][3] * in.w;
		y = matrix[1][0] * in.x
			+ matrix[1][1] * in.y
			+ matrix[1][2] * in.z
			+ matrix[1][3] * in.w;
		z = matrix[2][0] * in.x
			+ matrix[2][1] * in.y
			+ matrix[2][2] * in.z
			+ matrix[2][3] * in.w;
		w = matrix[3][0] * in.x
			+ matrix[3][1] * in.y
			+ matrix[3][2] * in.z
			+ matrix[3][3] * in.w;

		in.x = x;
		in.y = y;
		in.z = z;
		in.w = w;
	};

	void rotate_z(float theta)
	{
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		Matrix t1(cosTheta, -sinTheta, 0, 0,
			sinTheta, cosTheta, 0, 0,
			0, 0, 1.0f, 0,
			0, 0, 0, 1.0f);
		this->matrix = t1 * matrix;
	}

	// Rotate the object around the y axis
	void rotate_y(float theta)
	{
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		Matrix t1(cosTheta, 0, sinTheta, 0,
			0, 1.0f, 0, 0,
			-sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1.0f);
		this->matrix = t1 * matrix;
	}

	// Translate the object
	void translate(Vector translation)
	{
		Matrix t(1.0f, 0.0f, 0.0f, translation.x,
			0.0f, 1.0f, 0.0f, translation.y,
			0.0f, 0.0f, 1.0f, translation.z,
			0.0f, 0.0f, 0.0f, 1.0f);
		this->matrix = t * matrix;
	}
};
