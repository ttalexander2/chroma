#pragma once

#include <numeric>

namespace Chroma
{
	const size_t MAX_COMPONENTS = 64;
	const size_t MIN_ENTITIES = 512;

	/*
	* Entity - 32 bits
	*
	*	|32 bits				| 32 bits				|
	*	|unique_ID				| version				|
	*/


	typedef uint64_t EntityID;
	typedef uint32_t EntityIndex;
	typedef uint32_t EntityVersion;

	const uint64_t ENTITY_NULL = std::numeric_limits<uint64_t>::max();



}