#pragma once


#include "Chroma/Core/Log.h"
#include <string>
#include <map>
#include "Chroma/Utilities/Yaml.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include <entt.hpp>
#include "Chroma/Utilities/StringHash.h"
#include <Chroma/Core/TypeInfo.h>
#include "Chroma/Scene/EntityID.h"
#include "Chroma/Utilities/StringHash.h"
#include <entt.hpp>


namespace Polychrome
{
	class Inspector;
	class ComponentWidgets;
}


namespace Chroma
{
	class Scene;



#define CHROMA_COMPONENT(typeName, baseTypeName) \
		public: \
			using ClassName = typeName; \
			using BaseClass = baseTypeName; \
			typeName() = default; \
			typeName(EntityID id) : baseTypeName(id) {} \
			typeName(const typeName&) = default; \
			virtual StringHash GetType() const override { return GetTypeInfoStatic()->GetType(); } \
			virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
			virtual const TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
			virtual const size_t GetTypeSize() const override { return GetTypeSizeStatic(); } \
			static StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
			static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
			static const TypeInfo* GetTypeInfoStatic () { static const TypeInfo typeInfoStatic(#typeName, baseTypeName::GetTypeInfoStatic()); return &typeInfoStatic; } \
			static const size_t GetTypeSizeStatic() { return sizeof(#typeName); } \
			static void CreateReflectionModel(); \
			inline entt::meta_handle GetMetaHandle() override { return entt::meta_handle(*this); } \
			inline entt::meta_type GetMetaType() override { return entt::resolve<typeName>();} \
		private: \
			virtual void NOTYPEINFO() override {} \
		public: \

#define CHROMA_ABSTRACT_COMPONENT(typeName, baseTypeName) \
		public:\
			using ClassName = typeName; \
			using BaseClass = baseTypeName; \
			typeName() = default; \
			typeName(EntityID id) : baseTypeName(id) {} \
			typeName(const typeName&) = default; \
			virtual StringHash GetType() const override { return GetTypeInfoStatic()->GetType(); } \
			virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
			virtual const TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
			virtual const size_t GetTypeSize() const override { return GetTypeSizeStatic(); } \
			static StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
			static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
			static const TypeInfo* GetTypeInfoStatic () { static const TypeInfo typeInfoStatic(#typeName, baseTypeName::GetTypeInfoStatic()); return &typeInfoStatic; } \
			static const size_t GetTypeSizeStatic() { return sizeof(#typeName); } \
		public: \
		

	struct Component
	{
		//Delete base class constructors
		//Component() = delete;
		Component() : m_EntityID(ENTITY_NULL) { comparison_id = comparison_counter++; };
		Component(EntityID id) : m_EntityID(id) 
		{
			comparison_id = comparison_counter++;
		}
		Component(const Component&) = default;
		virtual ~Component() = default;

		virtual StringHash GetType() const { return GetTypeInfoStatic()->GetType(); }
		virtual const std::string& GetTypeName() const { return GetTypeInfoStatic()->GetTypeName(); }
		virtual const TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); }
		virtual const size_t GetTypeSize() const { return GetTypeSizeStatic(); }
		static StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); }
		static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); }
		static const TypeInfo* GetTypeInfoStatic() { static const TypeInfo typeInfoStatic("Component", nullptr); return &typeInfoStatic; }
		static const size_t GetTypeSizeStatic() { return sizeof(Component); }

		bool IsTypeOf(const TypeInfo* pTypeInfo) const;
		bool IsTypeOf(StringHash type) const;
		template<typename T>
		inline bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

		template<typename T>
		static bool IsOfTypeStatic() { return GetTypeStatic() == T::GetTypeStatic(); }

		const inline bool Valid() const { return m_EntityID != ENTITY_NULL; }

		const inline EntityID GetEntityID() const { return m_EntityID; }
		const inline unsigned int GetUniqueID() const { return comparison_id; }

		const bool IsEnabled() const { return m_Enabled; }
		const inline void Enable() { SetEnabled(true); }
		const inline void Disable() { SetEnabled(false); }
		virtual const inline void SetEnabled(bool enabled) { m_Enabled = enabled; }

		virtual entt::meta_handle GetMetaHandle() = 0;
		inline virtual entt::meta_type GetMetaType() = 0;


		virtual void Serialize(YAML::Emitter& out) {};

		virtual void Deserialize(YAML::Node& node) {};


		void DoSerialize(YAML::Emitter& out);
		void DoDeserialize(YAML::Node& node);

		inline int GetOrderID() const { return order_id; }


	protected:

		virtual void NOTYPEINFO() = 0;

	private:

		void BeginSerialize(YAML::Emitter& out);
		void EndSerialize(YAML::Emitter& out);


		EntityID m_EntityID;
		bool m_Enabled = true;


		/// @brief Whether the component is open in the editor inspector.
		bool editor_inspector_open = true;

		int order_id = 1;
		unsigned int comparison_id;
		static unsigned int comparison_counter;
		std::map<std::string, void*> _DataMembers;

		friend class Polychrome::ComponentWidgets;
		friend class Polychrome::Inspector;
		friend class Scene;
		friend class ECS;

		friend struct Tag;
		friend struct Transform;
		friend struct Relationship;
		friend struct EntityInfo;
	};

	template<typename T>
	concept ComponentType = std::is_base_of_v<Chroma::Component, T>;
				
}

