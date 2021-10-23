#pragma once

#include "entt.hpp"

namespace Chroma
{
	using EntityHandle = entt::entity;
	const EntityHandle ENTITY_NULL = entt::null;

	class Component;

	//Make sure its not abstract
	template<typename T>
	concept ComponentType = std::is_base_of<Chroma::Component, T>::value;
}