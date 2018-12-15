#pragma once

#include "vertex.h"
#include "colour.h"

// Representation of a photon to be stored in a photon map.
class Photon
{
public:
	Photon(Vertex pos, Colour power, Vector direction);

	Vertex pos;
	Colour power;
	Vector direction;
};

