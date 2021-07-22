#pragma once

#include <numeric>
#include "Chroma/Utilities/GUID.h"

namespace Chroma
{
	const size_t MAX_COMPONENTS = 64;
	const size_t MAX_ENTITIES = 2^32;

	// | 32 Bits - Unique ID |  32 - Bitwise component flag |

	typedef uint64_t EntityID;
	const EntityID ENTITY_NULL = std::numeric_limits<uint64_t>::max();

}