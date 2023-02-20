#pragma once


#include "Chroma/Core/Log.h"
#include "Chroma/Reflection/ComponentInitializer.h"

#include <string>
#include <map>
#include <concepts>
#include "Chroma/Utilities/Yaml.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include "Chroma/Utilities/StringHash.h"
#include <Chroma/Core/TypeInfo.h>
#include "Chroma/Scene/EntityID.h"
#include "Chroma/Utilities/StringHash.h"
#include "Chroma/Utilities/ComponentDataType.h"
#include "Chroma/Reflection/Reflection.h"


namespace Polychrome
{
	class Inspector;
	class ComponentWidgets;
}

namespace Chroma
{
	class Scene;
	struct Collider;

	struct Component
	{
		friend struct Collider;
		friend struct RigidBody;
		friend class Reflection::ComponentInitializer<Component>;
		friend class Reflection::Initializer;

		//I don't like macros and i've tried everything I could to avoid this macro, but unfortunately it is the cleanest
		//and simplest way to add reflection to components, and enforce how components behave.
#define CHROMA_COMPONENT(typeName, baseTypeName)																						\
		using ClassName = typeName;																										\
		using BaseClass = baseTypeName;																									\
		friend class Chroma::Reflection::ComponentInitializer<typeName>;																\
		friend class Reflection::Initializer;																							\
		typeName() : baseTypeName() { Initialize(); }																					\
		typeName(EntityID id) : baseTypeName(id) { Initialize(); }																		\
		typeName(const typeName &) = default;																							\
		typeName &operator=(const typeName &) = default;																				\
	private:																															\
		using type_factory = mirr::type_factory<typeName>;																				\
		static inline Reflection::ComponentInitializer<typeName> type_initializer{};													\
		static type_factory register_type();																							\
		static inline Reflection::type TypeInfo = Reflection::resolve<typeName>();														\
		virtual void NO_TYPE_INFO() override {}																							\
	public:																																\
		friend class Scene;																												\
		virtual Reflection::type GetType() const noexcept override { return TypeInfo; }													\
		virtual Reflection::handle ToHandle() const noexcept override { return mirr::handle(*((typeName*)this)); }											

#define CHROMA_ABSTRACT_COMPONENT(typeName, baseTypeName)																				\
		using ClassName = typeName;																										\
		using BaseClass = baseTypeName;																									\
		friend class Chroma::Reflection::ComponentInitializer<typeName>;																\
		friend class Reflection::Initializer;																							\
		typeName() : baseTypeName() { Initialize(); }																					\
		typeName(EntityID id) : baseTypeName(id) { Initialize(); }																		\
		typeName(const typeName &) = default;																							\
		typeName &operator=(const typeName &) = default;																				\
	private:																															\
		using type_factory = mirr::type_factory<typeName>;																				\
		static inline Reflection::ComponentInitializer<typeName> type_initializer{};													\
		static type_factory register_type();																							\
		static inline Reflection::type TypeInfo = Reflection::resolve<typeName>();														\
		virtual void NO_TYPE_INFO() override {}																							\
																																		\
	public:																																\
		friend class Scene;																												\
		virtual Reflection::type GetType() const noexcept override { return TypeInfo; }													\
		virtual Reflection::handle ToHandle() const noexcept override { return mirr::handle(*((typeName*)this)); }

		Component() :
			m_EntityID(ENTITY_NULL)
		{
			comparison_id = comparison_counter++;
		};

		Component(EntityID id) :
			m_EntityID(id)
		{
			comparison_id = comparison_counter++;
		}

		Component(const Component &) = default;
		virtual ~Component() = default;

		virtual void Initialize()
		{
		};

		const bool Valid() const { return m_EntityID != ENTITY_NULL; }

		const EntityID GetEntityID() const { return m_EntityID; }
		const unsigned int GetUniqueID() const { return comparison_id; }

		const bool IsEnabled() const { return m_Enabled; }
		const void Enable() { SetEnabled(true); }
		const void Disable() { SetEnabled(false); }
		virtual const void SetEnabled(bool enabled) { m_Enabled = enabled; }

		int GetOrderID() const { return order_id; }

		virtual Reflection::type GetType() const noexcept { return this->TypeInfo; }
		virtual Reflection::handle ToHandle() const noexcept { return mirr::handle(*this); }

	protected:
		virtual void NO_TYPE_INFO() = 0;

	private:
		using type_factory = Reflection::type_factory<Component>;
		static inline Reflection::ComponentInitializer<Component> type_initializer{};
		static type_factory register_type();
		static inline Reflection::type TypeInfo = Reflection::resolve<Component>();

		EntityID m_EntityID;
		bool m_Enabled = true;


		/// @brief Whether the component is open in the editor inspector.
		bool editor_inspector_open = true;

		int order_id = 1;
		unsigned int comparison_id;
		static unsigned int comparison_counter;

		friend class Polychrome::ComponentWidgets;
		friend class Polychrome::Inspector;

		friend class Scene;
		friend class ECS;

		friend struct Tag;
		friend struct Transform;
		friend struct Relationship;
		friend struct EntityInfo;
		friend struct ComponentRegistry;
	};

#ifdef CHROMA_DEBUG

	template <typename T>
	concept ComponentType = std::is_base_of_v<Component, T>;

#else
	#define ComponentType typename
#endif
}
