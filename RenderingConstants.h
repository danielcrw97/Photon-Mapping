#pragma once
// Options for rendering, defined in RenderingConstants.cpp
class RenderingConstants
{
public:
	static bool const USE_AA_SAMPLING;
	static bool const USE_SHADOW_SAMPLING;
	static bool const CREATE_CAUSTIC_MAP;
	static bool const CREATE_GLOBAL_MAP;

	// The amount of samples to use for AA will be this squared.
	static int const AA_SAMPLE_GRID_SIZE;

	// The amount of samples to use for shadow rays will be this squared.
	static int const SHADOW_SAMPLE_GRID_SIZE;

	// The amount of difference in a red, blue or green colour to use anti-aliasing.
	static float const ALIASING_COLOUR_DIF_CHECK;

	// The refraction index of glass
	static float const GLASS_REFRACTION_INDEX;

	// Multiplier for global photons
	static float const GLOBAL_PHOTON_POWER;

	// Multiplier for caustic photons
	static float const CAUSTIC_PHOTON_POWER;
};

