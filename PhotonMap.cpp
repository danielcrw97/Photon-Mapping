
#include "PhotonMap.h"
#include "PhotonMapNode.h"
#include "Photon.h"
#include "MathUtils.h"
#include <algorithm>

using namespace std;

PhotonMap::PhotonMap()
{
	this->root = nullptr;
	this->size = 0;
}

PhotonMap::PhotonMap(PhotonList& photons)
{
	this->size = (int)photons.size();
	PhotonMapNode rootNode;
	buildBalancedTree(rootNode, photons, 0);
	this->root = make_shared<PhotonMapNode>(rootNode);
}

Colour PhotonMap::irradianceEstimate(Hit const& hit, int k, bool filtered)
{
	float const FILTER_COEFFICIENT = 1.1;
	Colour flux;
	NearestNodeHeap heap;
	float maxDistance = MAX_IRRADIANCE_DISTANCE;
	locatePhotons(root, hit.position, maxDistance, k, heap);

	if (!heap.empty())
	{
		float r = (*(heap.end() - 1)).distance;
		for (NearestPhoton photon : heap)
		{
			shared_ptr<Photon> photonP = photon.node->value;
			Vector photonDir = photonP->direction;
			float diff = hit.normal.dot(photonDir);
			if (diff < 0.0f)
			{
				Colour pow = photonP->power;
				if (filtered)
				{
					float weight = 1 - ((photon.distance) / (FILTER_COEFFICIENT * r));
					pow.scaleColour(weight);
				}
				flux.add(pow);
			}
		}
		flux.scaleColour(1.0f / (MathConstants::PI * (r * r)));
		if (filtered)
		{
			flux.scaleColour(1 - (2.0f / (3.0f * FILTER_COEFFICIENT)));
		}
	}
	return flux;
}

void PhotonMap::locatePhotons(shared_ptr<PhotonMapNode> node, Vertex const & pos, float& maxDistance, int const numPhotons, NearestNodeHeap& photons)
{
	if (node == nullptr)
	{
		return;
	}

	float signedDistance = PhotonMap::getNodeDistance(pos, node);

	// Do recursive child stuff.
	if (signedDistance < 0)
	{
		locatePhotons(node->left, pos, maxDistance, numPhotons, photons);
		// Only bother with the right subtree if it can have closer nodes.
		if ((signedDistance * signedDistance) < maxDistance)
		{
			locatePhotons(node->right, pos, maxDistance, numPhotons, photons);
		}
	}
	else
	{
		locatePhotons(node->right, pos, maxDistance, numPhotons, photons);
		// Only bother with the left subtree if it can have closer nodes
		if ((signedDistance * signedDistance) < maxDistance)
		{
			locatePhotons(node->left, pos, maxDistance, numPhotons, photons);
		}
	}

	float realDistance = powf(node->value->pos.x - pos.x, 2.0f) + powf(node->value->pos.y - pos.y, 2.0f) + powf(node->value->pos.z - pos.z, 2.0f);

	if (realDistance < maxDistance)
	{
		NearestPhoton np;
		np.node = node;
		np.distance = realDistance;

		bool added = false;
		for (int i = 0; i < (int)photons.size(); ++i)
		{
			if (np.distance < photons.at(i).distance)
			{
				photons.insert(photons.begin() + i, np);
				added = true;
				break;
			}
		}

		// If it has not been added, add it to the end of the photon list.
		if (!added)
		{
			photons.push_back(np);
		}

		// If the list is full, pop off the last photon (the one with the biggest distance) and update the maxDistance.
		if ((int) photons.size() > numPhotons)
		{
			// Adjust max distance to prune search.
			photons.erase(photons.end() - 1);

			maxDistance = photons.at((int)photons.size() - 1).distance;
		}
	}
}

PhotonMap::~PhotonMap()
{
}

void PhotonMap::buildBalancedTree(PhotonMapNode& node, PhotonList& photons, int depth)
{
	if ((int)photons.size() == 0)
	{
		return;
	}

	node.depth = depth;

	float xMin = (*std::min_element(photons.begin(), photons.end(), xComparator))->pos.x;
	float yMin = (*std::min_element(photons.begin(), photons.end(), yComparator))->pos.y;
	float zMin = (*std::min_element(photons.begin(), photons.end(), zComparator))->pos.z;
	float xMax = (*std::max_element(photons.begin(), photons.end(), xComparator))->pos.x;
	float yMax = (*std::max_element(photons.begin(), photons.end(), yComparator))->pos.y;
	float zMax = (*std::max_element(photons.begin(), photons.end(), zComparator))->pos.z;

	float xDistance = xMax - xMin;
	float yDistance = yMax - yMin;
	float zDistance = zMax - yMin;

	// Choose the plane split depending on the max distance. 
	NodeSplittingPlane split = (xDistance >= yDistance && xDistance >= zDistance) ?
		NodeSplittingPlane::XDIM : (yDistance >= zDistance) ? NodeSplittingPlane::YDIM : NodeSplittingPlane::ZDIM;

	node.plane = split;
	if (split == NodeSplittingPlane::XDIM)
	{
		std::sort(photons.begin(), photons.end(), xComparator);
	}
	else if (split == NodeSplittingPlane::YDIM)
	{
		std::sort(photons.begin(), photons.end(), yComparator);
	}
	else if (split == NodeSplittingPlane::ZDIM)
	{
		std::sort(photons.begin(), photons.end(), zComparator);
	}

	int medianIndex = floor(photons.size() / 2);
	node.value = photons.at(medianIndex);

	PhotonList left;
	PhotonList right;

	for (int i = 0; i < medianIndex; ++i)
	{
		left.push_back(photons.at(i));
	}
	for (int j = medianIndex + 1; j < (int)photons.size(); ++j)
	{
		right.push_back(photons.at(j));
	}

	// These photons are not needed any more.
	// Clear them to free up memory.
	photons.clear();

	if (left.empty() && right.empty())
	{
		node.isLeaf = true;
		node.left = nullptr;
		node.right = nullptr;
		return;
	}
	else if (left.empty())
	{
		node.left = nullptr;
		PhotonMapNode rightNode;
		buildBalancedTree(rightNode, right, ++depth);
		node.right = make_shared<PhotonMapNode>(rightNode);
	}
	else if (right.empty())
	{
		node.right = nullptr;
		PhotonMapNode leftNode;
		buildBalancedTree(leftNode, left, ++depth);
		node.left = make_shared<PhotonMapNode>(leftNode);
	}
	else
	{
		PhotonMapNode rightNode;
		PhotonMapNode leftNode;
		buildBalancedTree(leftNode, left, ++depth);
		buildBalancedTree(rightNode, right, ++depth);
		node.left = make_shared<PhotonMapNode>(leftNode);
		node.right = make_shared<PhotonMapNode>(rightNode);
	}
}

float PhotonMap::getNodeDistance(Vertex const& point, shared_ptr<PhotonMapNode> node)
{
	NodeSplittingPlane split = node->plane;
	float signedDistance;
	if (split == NodeSplittingPlane::XDIM)
	{
		signedDistance = point.x - node->value->pos.x;
	}
	else if (split == NodeSplittingPlane::YDIM)
	{
		signedDistance = point.y - node->value->pos.y;
	}
	else if (split == NodeSplittingPlane::ZDIM)
	{
		signedDistance = point.z - node->value->pos.z;
	}
	return signedDistance;
}

bool PhotonMap::xComparator(PhotonPointer p1, PhotonPointer p2)
{
	return p1->pos.x < p2->pos.x;
}

bool PhotonMap::yComparator(PhotonPointer p1, PhotonPointer p2)
{
	return p1->pos.y < p2->pos.y;
}

bool PhotonMap::zComparator(PhotonPointer p1, PhotonPointer p2)
{
	return p1->pos.z < p2->pos.z;
}
