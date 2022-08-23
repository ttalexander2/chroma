#pragma once


#include "Chroma/Core/Log.h"
#include <string>
#include <map>
#include <concepts>
#include "Chroma/Utilities/Yaml.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include <entt.hpp>
#include "Chroma/Utilities/StringHash.h"
#include <Chroma/Core/TypeInfo.h>
#include "Chroma/Scene/EntityID.h"
#include "Chroma/Utilities/StringHash.h"
#include "Chroma/Utilities/ComponentDataType.h"
#include "Chroma/Reflection/Reflection.h"


#ifdef CHROMA_EDITOR
namespace Polychrome
{
	class Inspector;
	class ComponentWidgets;
}
#endif

namespace Chroma
{
	class Scene;
	class Collider;

	struct Component
	{
		friend class Collider;
		friend class RigidBody;

		//I don't like macros and i've tried everything I could to avoid this macro, but unfortunely it is the cleanest
		//and simplest way to add reflection to components, and enforce how components behave.
#define CHROMA_COMPONENT(typeName, baseTypeName)																				\
		using ClassName = typeName;																								\
		using BaseClass = baseTypeName;																							\
		typeName() : baseTypeName() { Initialize(); }																			\
		typeName(EntityID id) : baseTypeName(id) { Initialize(); }																\
		typeName(const typeName &) = default;																					\
		typeName &operator=(const typeName &) = default;																		\
		private:																												\
			static inline Reflection::TypeData<typeName> TypeInfo{};															\
			static Reflection::TypeFactory<typeName> RegisterType();															\
			virtual void NO_TYPE_INFO() override {}																				\
	public:																														\
		virtual inline const size_t TypeId() const override { return Reflection::Resolve<typeName>().Id(); }					\
		virtual inline const std::string TypeName() const override { return Reflection::Resolve<typeName>().GetName(); }		\
		virtual inline const Reflection::Type GetType() const override { return Reflection::Resolve<typeName>(); }				\
		virtual inline Reflection::Any ToAny() const override { return Reflection::Any(*this); }								\
		inline bool IsAbstract() const override { return false; }																\
		friend class Scene;																										\
		friend class Reflection;

#define CHROMA_ABSTRACT_COMPONENT(typeName, baseTypeName)																		\
		using ClassName = typeName;																								\
		using BaseClass = baseTypeName;																							\
		typeName() : baseTypeName() { Initialize(); }																			\
		typeName(EntityID id) : baseTypeName(id) { Initialize(); }																\
		typeName(const typeName &) = default;																					\
		typeName &operator=(const typeName &) = default;																		\
																																\
	private:																													\
		static inline Reflection::TypeData<typeName> TypeInfo{};																\
		static Reflection::TypeFactory<typeName> RegisterType();																\
																																\
	public:																														\
		virtual inline const size_t TypeId() const override { return Reflection::Resolve<typeName>().Id(); }					\
		virtual inline const std::string TypeName() const override { return Reflection::Resolve<typeName>().GetName(); }		\
		virtual inline const Reflection::Type GetType() const override { return Reflection::Resolve<typeName>(); }				\
		virtual inline Reflection::Any ToAny() const override { return Reflection::Any(*this); }								\
		friend class Scene;																										\
		friend class Reflection;

		
		Component() :
				m_EntityID(ENTITY_NULL)
		{
			comparison_id = comparison_counter++;
		};

		Component(EntityID id) : m_EntityID(id) 
		{
			comparison_id = comparison_counter++;
		}

		Component(const Component&) = default;
		virtual ~Component() = default;

		virtual void Initialize() {};

		const inline bool Valid() const { return m_EntityID != ENTITY_NULL; }

		const inline EntityID GetEntityID() const { return m_EntityID; }
		const inline unsigned int GetUniqueID() const { return comparison_id; }

		const inline bool IsEnabled() const { return m_Enabled; }
		const inline void Enable() { SetEnabled(true); }
		const inline void Disable() { SetEnabled(false); }
		virtual const inline void SetEnabled(bool enabled) { m_Enabled = enabled; }

		inline int GetOrderID() const { return order_id; }

		virtual inline const size_t TypeId() const { return Reflection::Resolve<Component>().Id(); }
		virtual inline const std::string TypeName() const { return Reflection::Resolve<Component>().GetName(); }
		virtual inline const Reflection::Type GetType() const { return Reflection::Resolve<Component>(); }
		virtual inline Reflection::Any ToAny() const { return Reflection::Any(this); }
		virtual inline bool IsAbstract() const { return true; }

		template <typename T> 
		inline const bool IsOfType() const
		{
			return Reflection::Resolve<T>().Id() == TypeId();
		}
		
		inline const bool IsOfType(size_t id) const
		{
			return TypeId() == id;
		}
		
		inline const bool IsOfType(const std::string& type_name) const
		{
			return type_name.compare(TypeName()) == 0;
		}

	protected:
		virtual void NO_TYPE_INFO() = 0;

	private:
		static inline Reflection::TypeData<Component> TypeInfo{};
		static Reflection::TypeFactory<Component> RegisterType();

		EntityID m_EntityID;
		bool m_Enabled = true;


		/// @brief Whether the component is open in the editor inspector.
		bool editor_inspector_open = true;

		int order_id = 1;
		unsigned int comparison_id;
		static unsigned int comparison_counter;

#ifdef CHROMA_EDITOR
		friend class Polychrome::ComponentWidgets;
		friend class Polychrome::Inspector;
#endif

		friend class Scene;
		friend class ECS;

		friend struct Tag;
		friend struct Transform;
		friend struct Relationship;
		friend struct EntityInfo;
		friend class Reflection;
	};

	template <typename T>
	concept ComponentType = std::is_base_of_v<Chroma::Component, T>;
				
}

