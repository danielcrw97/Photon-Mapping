#pragma once
#include <memory>
#include "Photon.h"

enum class NodeSplittingPlane
{
	XDIM,
	YDIM,
	ZDIM
};

class PhotonMapNode
{

public:
	// Left subnode
	std::shared_ptr<PhotonMapNode> left;

	// Right subnode
	std::shared_ptr<PhotonMapNode> right;

	// Pointer to the actual photon value
	std::shared_ptr<Photon> value;

	// What plane the node was sorted on.
	NodeSplittingPlane plane;
	
	int depth;
	bool isLeaf;

	PhotonMapNode()
	{
		depth = 0;
		isLeaf = false;
	}
};

