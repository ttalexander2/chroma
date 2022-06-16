#pragma once

#include <string>
#include <entt.hpp>

#include <Chroma/Components/Transform.h>
#include <Chroma/Scene/Component.h>
#include <Chroma/Utilities/StringHash.h>

namespace Chroma
{
	class Reflection
	{
	public:

		/// <summary>
		/// This function will initialize all standard serialization types into the reflection system.
		/// </summary>
		static void InitializeDataTypes();


		/// <summary>
		/// Registers a single data type into the reflection system. This function does not decompose the type into
		/// properties/functions.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T>
		static void RegisterDataType(const std::string& type_name, std::function<void(YAML::Emitter&, entt::meta_any&)> serialize_function, std::function<entt::meta_any(YAML::Node&)> deserialize_function)
		{
			size_t hash = StringHash::Hash(type_name);
			entt::meta<T>().type(hash);
			size_t id = entt::resolve<T>().id();
			s_SerializeValueFunctions[id] = serialize_function;
			s_DeserializeValueFunctions[id] = deserialize_function;
			s_DataTypeNames[id] = type_name;
		}

		template <typename Type>
		static void RegisterComponent()
		{
			static_assert(std::is_base_of<Chroma::Component, Type>::value, "Type T is not derived from Component!");
			size_t hash = StringHash::Hash(Type::GetTypeNameStatic());
			auto meta = entt::meta<Type>().type(hash);
			size_t id = entt::resolve<Type>().id();
			s_ComponentMeta[id] = meta;
			s_ComponentNames[id] = Type::GetTypeNameStatic();
			s_ComponentPropertyNames[id] = std::unordered_map<size_t, std::string>();
		}

		template <typename ComponentType, auto Property>
		static void RegisterComponentProperty(const std::string& property_name, bool serialize = true)
		{
			static_assert(std::is_base_of<Chroma::Component, ComponentType>::value, "Type T is not derived from Component!");

			//Get component id
			size_t id = entt::resolve<ComponentType>().id();

			//Get string hash for property
			size_t prop_hash = StringHash::Hash(property_name);

			//Register component if not registered
			if (s_ComponentMeta.find(id) == s_ComponentMeta.end())
				RegisterComponent<ComponentType>();

			//Obtain meta factory from component meta storage
			auto meta_factory = entt::any_cast<entt::meta_factory<ComponentType>>(&s_ComponentMeta[id]);

			//Add property and reassign metta
			s_ComponentMeta[id] = meta_factory->data<Property>(prop_hash);

			//Set property name
			auto property_id = entt::resolve<ComponentType>().data(prop_hash).id();
			s_ComponentPropertyNames[id][property_id] = property_name;
		}

		template <typename ComponentType, auto Setter, auto Getter>
		static void RegisterComponentProperty(const std::string& property_name, bool serialize = true)
		{
			static_assert(std::is_base_of<Chroma::Component, ComponentType>::value, "Type T is not derived from Component!");

			if (Setter == nullptr || Getter == nullptr)
				serialize = false;

			//Get component id
			size_t id = entt::resolve<ComponentType>().id();

			//Get string hash for property
			size_t prop_hash = StringHash::Hash(property_name);

			//Register component if not registered
			if (s_ComponentMeta.find(id) == s_ComponentMeta.end())
				RegisterComponent<ComponentType>();

			//Obtain meta factory from component meta storage
			auto meta_factory = entt::any_cast<entt::meta_factory<ComponentType>>(&s_ComponentMeta[id]);

			//Add property and reassign metta
			s_ComponentMeta[id] = meta_factory->data<Setter, Getter>(prop_hash);

			//Set property name
			auto property_id = entt::resolve<ComponentType>().data(prop_hash).id();
			s_ComponentPropertyNames[id][property_id] = property_name;
		}

		template <typename ComponentType, auto Function>
		static void RegisterComponentFunction(const std::string& function_name)
		{
			static_assert(std::is_base_of<Chroma::Component, ComponentType>::value, "Type T is not derived from Component!");

			//Get component id
			size_t id = entt::resolve<ComponentType>().id();

			//Get string hash for property
			size_t func_hash = StringHash::Hash(function_name);

			//Register component if not registered
			if (s_ComponentMeta.find(id) == s_ComponentMeta.end())
				RegisterComponent<ComponentType>();

			//Obtain meta factory from component meta storage
			auto meta_factory = entt::any_cast<entt::meta_factory<ComponentType>>(&s_ComponentMeta[id]);

			//Add property and reassign metta
			s_ComponentMeta[id] = meta_factory->func<Function>(func_hash);

			//Set property name
			auto property_id = entt::resolve<ComponentType>().func(func_hash).id();
			s_ComponentPropertyNames[id][property_id] = function_name;
		}



		static std::string ResolveComponentID(entt::id_type component_type);
		static std::string ResolveComponentPropertyID(entt::id_type component_type, entt::id_type property_type);
		static std::string ResolveComponentPropertyID(const std::string& component_type, entt::id_type property_type);


		static bool SerializeProperty(YAML::Emitter& emitter, const std::string& name, entt::meta_any& value);
		static bool SerializeValue(YAML::Emitter& emitter, entt::meta_any& value);
		static entt::meta_any DeserializeValue(YAML::Node& node);

		//static bool SerializeComponent(YAML::Emitter& emitter, entt::id_type component_type, entt::meta_handle component_handle);
		//static entt::meta_any DeserializeComponent(YAML::Node& node);




	private:
		static std::unordered_map<size_t, entt::any> s_ComponentMeta;
		static std::unordered_map<size_t, std::string> s_ComponentNames;
		static std::unordered_map<size_t, std::unordered_map<size_t, std::string>> s_ComponentPropertyNames;
		static std::unordered_map<size_t, std::unordered_map<size_t, bool>> s_ComponentPropertySerialize;
		static std::unordered_map<size_t, std::string> s_DataTypeNames;
		static std::unordered_map<size_t, std::function<entt::meta_any(YAML::Node&)>> s_DeserializeValueFunctions;
		static std::unordered_map<size_t, std::function<void(YAML::Emitter&, entt::meta_any&)>> s_SerializeValueFunctions;

	};
}