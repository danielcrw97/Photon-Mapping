#include "RenderingConstants.h"


// Options for rendering are found here!
bool const RenderingConstants::USE_AA_SAMPLING = true;
bool const RenderingConstants::USE_SHADOW_SAMPLING = true;
bool const RenderingConstants::CREATE_CAUSTIC_MAP = true;
bool const RenderingConstants::CREATE_GLOBAL_MAP = true;
int const RenderingConstants::AA_SAMPLE_GRID_SIZE = 2;
int const RenderingConstants::SHADOW_SAMPLE_GRID_SIZE = 6;
float const RenderingConstants::ALIASING_COLOUR_DIF_CHECK = 0.25f;
float const RenderingConstants::GLASS_REFRACTION_INDEX = 2.40f;
float const RenderingConstants::GLOBAL_PHOTON_POWER = 25.0f;
float const RenderingConstants::CAUSTIC_PHOTON_POWER = 0.6f;