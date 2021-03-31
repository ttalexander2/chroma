#pragma once

namespace Chroma
{
	const size_t MAX_COMPONENTS = 64;
	const size_t MIN_ENTITIES = 512;

	/*
	* EntityID - 32 bits
	*
	*	|32 bits				| 32 bits				|
	*	|unique_ID				| version				|
	*/

	typedef unsigned long long EntityID;
	typedef unsigned long EntityIndex;
	typedef unsigned long EntityVersion;
	typedef unsigned int ComponentTypeID;
}