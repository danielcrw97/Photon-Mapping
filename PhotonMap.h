#pragma once
#include <vector>
#include <memory>
#include "Photon.h"
#include <list>
#include "hit.h"

// Forward declare the photon map node.
class PhotonMapNode;

float const MAX_IRRADIANCE_DISTANCE = 1.0f;

struct NearestPhoton
{
	std::shared_ptr<PhotonMapNode> node;
	float distance;
};

typedef std::shared_ptr<Photon> PhotonPointer;
typedef std::vector<std::shared_ptr<Photon>> PhotonList;
typedef std::vector<NearestPhoton> NearestNodeHeap;

// A balanced KD tree set up to easily search for nearest neighbour photons during rendering.
class PhotonMap
{
private:

	static void buildBalancedTree(PhotonMapNode& node, PhotonList& remainingPhotons, int depth);
	static float getNodeDistance(Vertex const& point, std::shared_ptr<PhotonMapNode> node);
	static bool xComparator(PhotonPointer p1, PhotonPointer p2);
	static bool yComparator(PhotonPointer p1, PhotonPointer p2);
	static bool zComparator(PhotonPointer p1, PhotonPointer p2);

public:
	int size;
	std::shared_ptr<PhotonMapNode> root;

	PhotonMap();

	// Create a balnced KD tree of photons from a list of photon objects.
	PhotonMap(PhotonList& photons);

	// Estimate irradiance from a given surface position, where k is the number of nearest photons.
	Colour irradianceEstimate(Hit const& hit, int k, bool filtered);

	void locatePhotons(std::shared_ptr<PhotonMapNode> node, Vertex const& pos, float& maxDistance, int const numPhotons, NearestNodeHeap& photons);

	~PhotonMap();
};
