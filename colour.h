/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Colour is a class to store and maniplulate an rgba colour.

#pragma once

#include <vector>
#include <cmath>

class Colour {
public:
	float r;
	float g;
	float b;
	float a;

	Colour()
	{
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		a = 1.0f;
	}

	Colour(float pr, float pg, float pb, float pa)
	{
		r = pr;
		g = pg;
		b = pb;
		a = pa;
	}

	Colour(float pr, float pg, float pb)
	{
		r = pr;
		g = pg;
		b = pb;
		a = 1.0f;
	}

	void scale(Colour &scaling)
	{
		r *= scaling.r;
		g *= scaling.g;
		b *= scaling.b;
		a *= scaling.a;
	}

	void scaleColour(float val)
	{
		r *= val;
		g *= val;
		b *= val;
	}

	void add(Colour &adjust)
	{
		r += adjust.r;
		g += adjust.g;
		b += adjust.b;
	}

	// Return the absolute difference in colour.
	Colour operator-(Colour const& right)
	{
		return Colour(fabsf(r - right.r), fabsf(g - right.g), fabsf(b - right.b));
	}

	// Make sure all colour values fall between [0, 1].
	void clamp()
	{
		if (r < 0.0f)
		{
			r = 0.0f;
		}
		if (g < 0.0f)
		{
			g = 0.0f;
		}
		if (b < 0.0f)
		{
			b = 0.0f;
		}
		if (r > 1.0f)
		{
			r = 1.0f;
		}
		if (g > 1.0f)
		{
			g = 1.0f;
		}
		if (b > 1.0f)
		{
			b = 1.0f;
		}
	}

	static void average(std::vector<Colour>& colours, Colour& averageColour)
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;

		for (Colour colour : colours)
		{
			r += colour.r;
			g += colour.g;
			b += colour.b;
		}

		int size = colours.size();
		averageColour.r = r / (float)size;
		averageColour.g = g / (float)size;
		averageColour.b = b / (float)size;
	}
	
};
