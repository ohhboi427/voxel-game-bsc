#pragma once

#include "../utility/Octree.h"

/**
 * @brief A 32*32*32 sized slice of the world.
 */
using Chunk = Octree<5u>;
