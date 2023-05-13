#pragma once
#include <entt/entt.hpp>

namespace Chroma
{
	using EntityID = entt::entity;
	constexpr EntityID EntityNull = entt::null;

	#define ENTITY_NULL EntityNull
}
