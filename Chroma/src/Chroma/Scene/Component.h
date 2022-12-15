#pragma once


#include "Chroma/Core/Log.h"
#include <string>
#include <map>
#include <concepts>
#include "Chroma/Utilities/Yaml.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include <entt/entt.hpp>
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

		//I don't like macros and i've tried everything I could to avoid this macro, but unfortunately it is the cleanest
		//and simplest way to add reflection to components, and enforce how components behave.
#define CHROMA_COMPONENT(typeName, baseTypeName)																						\
		using ClassName = typeName;																										\
		using BaseClass = baseTypeName;																									\
		typeName() : baseTypeName() { Initialize(); }																					\
		typeName(EntityID id) : baseTypeName(id) { Initialize(); }																		\
		typeName(const typeName &) = default;																							\
		typeName &operator=(const typeName &) = default;																				\
		private:																														\
		static inline Reflection::TypeData TypeInfo = Reflection::TypeData::Create<typeName>();											\
	static Reflection::TypeFactory<typeName> RegisterType();																			\
			virtual void NO_TYPE_INFO() override {}																						\
	public:																																\
		virtual inline const uint32_t TypeId() const override { return Reflection::Resolve<typeName>().Id(); }							\
		virtual inline const std::string TypeName() const override { return Reflection::Resolve<typeName>().GetName(); }				\
		virtual inline const Reflection::Type GetType() const override { return Reflection::Resolve<typeName>(); }						\
		virtual inline Reflection::AnyRef ToAnyRef() const override { return Reflection::AnyRef::Create(this); }						\
		virtual inline Reflection::Any ToAny() const override { return Reflection::Any(*this); }										\
		virtual inline Reflection::Handle ToHandle() const { return Reflection::Handle(*this); }										\
		inline bool IsAbstract() const override { return false; }																		\
		friend class Scene;																												\
		friend struct Reflection;

#define CHROMA_ABSTRACT_COMPONENT(typeName, baseTypeName)																				\
		using ClassName = typeName;																										\
		using BaseClass = baseTypeName;																									\
		typeName() : baseTypeName() { Initialize(); }																					\
		typeName(EntityID id) : baseTypeName(id) { Initialize(); }																		\
		typeName(const typeName &) = default;																							\
		typeName &operator=(const typeName &) = default;																				\
	private:																															\
		static inline Reflection::TypeData TypeInfo = Reflection::TypeData::Create<typeName>();											\
		static Reflection::TypeFactory<typeName> RegisterType();																		\
																																		\
	public:																																\
		virtual inline const uint32_t TypeId() const override { return Reflection::Resolve<typeName>().Id(); }							\
		virtual inline const std::string TypeName() const override { return Reflection::Resolve<typeName>().GetName(); }				\
		virtual inline const Reflection::Type GetType() const override { return Reflection::Resolve<typeName>(); }						\
		virtual inline Reflection::AnyRef ToAnyRef() const override { return Reflection::AnyRef::Create(this); }						\
		virtual inline Reflection::Any ToAny() const override { return Reflection::Any(*this); }										\
		virtual inline Reflection::Handle ToHandle() const { return Reflection::Handle(*this); }										\
		friend class Scene;																												\
		friend struct Reflection;

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

		virtual const uint32_t TypeId() const { return Reflection::Resolve<Component>().Id(); }
		virtual const std::string TypeName() const { return Reflection::Resolve<Component>().GetName(); }
		virtual const Reflection::Type GetType() const { return Reflection::Resolve<Component>(); }
		virtual Reflection::AnyRef ToAnyRef() const { return Reflection::AnyRef::Create(this); }
		virtual Reflection::Any ToAny() const { return Reflection::Any(*this); }
		virtual Reflection::Handle ToHandle() const { return Reflection::Handle(*this); }
		virtual bool IsAbstract() const { return true; }

		template <typename T>
		const bool IsType() const
		{
			return Reflection::Resolve<T>().Id() == TypeId();
		}

		const bool IsType(uint32_t id) const
		{
			return TypeId() == id;
		}

		const bool IsType(const std::string &type_name) const
		{
			return type_name.compare(TypeName()) == 0;
		}

	protected:
		virtual void NO_TYPE_INFO() = 0;

	private:
		static inline Reflection::TypeData TypeInfo = Reflection::TypeData::Create<Component>();
		static Reflection::TypeFactory<Component> RegisterType();

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
		friend struct Reflection;
		friend struct ComponentRegistry;
	};

#ifdef CHROMA_DEBUG

	template <typename T>
	concept ComponentType = std::is_base_of_v<Component, T>;

#else
	#define ComponentType typename
#endif
}
