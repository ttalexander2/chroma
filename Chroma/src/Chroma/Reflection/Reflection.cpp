#include "chromapch.h"

#include "Reflection.h"
#include "Chroma/Components/Collider.h"


namespace Chroma
{
	std::unordered_map<size_t, entt::any> Reflection::s_ComponentMeta;
	std::unordered_map<size_t, std::string> Reflection::s_ComponentNames;
	std::unordered_map<size_t, std::string> Reflection::s_DataTypeNames;
	std::unordered_map<size_t, std::unordered_map<size_t, std::string>> Reflection::s_ComponentPropertyNames;
	std::unordered_map<size_t, std::unordered_map<size_t, bool>>  Reflection::s_ComponentPropertySerialize;
	std::unordered_map<size_t, std::function<entt::meta_any(YAML::Node&)>> Reflection::s_DeserializeValueFunctions;
	std::unordered_map<size_t, std::function<void(YAML::Emitter&, entt::meta_any&)>> Reflection::s_SerializeValueFunctions;

#define REGISTER_TYPE_AUTO(type) RegisterDataType<type>(#type,\
			[&](YAML::Emitter& emitter, entt::meta_any& value) {\
				emitter << value.cast<type>();\
			},\
				[&](YAML::Node& node) {\
				return node.as<type>();\
			})

#define REGISTER_TYPE(type, name) RegisterDataType<type>(name,\
			[&](YAML::Emitter& emitter, entt::meta_any& value) {\
				emitter << value.cast<type>();\
			},\
				[&](YAML::Node& node) {\
				return node.as<type>();\
			})


	void Reflection::InitializeDataTypes()
	{
		REGISTER_TYPE_AUTO(bool);
		REGISTER_TYPE_AUTO(float);
		REGISTER_TYPE_AUTO(double);
		REGISTER_TYPE(int32_t, "int32");
		REGISTER_TYPE(uint32_t, "uint32");
		REGISTER_TYPE(int64_t, "int64");
		REGISTER_TYPE(uint64_t, "uint64");
		REGISTER_TYPE_AUTO(char);
		REGISTER_TYPE(unsigned char, "uchar");
		REGISTER_TYPE(std::string, "string");
		REGISTER_TYPE(Chroma::Ref<Chroma::Asset>, "asset");
		REGISTER_TYPE(Math::vec2, "vec2");
		REGISTER_TYPE(Math::vec3, "vec3");
		REGISTER_TYPE(Math::vec4, "vec4");
		REGISTER_TYPE(Chroma::EntityID, "entity_id");
		REGISTER_TYPE(Chroma::GUID, "guid");
	}

	std::string Reflection::ResolveComponentID(entt::id_type component_type)
	{
		if (s_ComponentNames.find(component_type) == s_ComponentNames.end())
			return std::string();

		return s_ComponentNames[component_type];
	}


	std::string Reflection::ResolveComponentPropertyID(entt::id_type component_type, entt::id_type property_type)
	{
		if (s_ComponentNames.find(component_type) == s_ComponentNames.end() || s_ComponentPropertyNames.find(component_type) == s_ComponentPropertyNames.end() || s_ComponentPropertyNames[component_type].find(property_type) == s_ComponentPropertyNames[component_type].end())
			return std::string();

		return s_ComponentPropertyNames[component_type][property_type];
	}


	std::string Reflection::ResolveComponentPropertyID(const std::string& component_type, entt::id_type property_type)
	{
		size_t hash = entt::resolve(StringHash::Hash(component_type)).id();

		if (s_ComponentNames.find(hash) == s_ComponentNames.end() || s_ComponentPropertyNames.find(hash) == s_ComponentPropertyNames.end() || s_ComponentPropertyNames[hash].find(property_type) == s_ComponentPropertyNames[hash].end())
			return std::string();

		return s_ComponentPropertyNames[hash][property_type];
	}


	bool Reflection::SerializeProperty(YAML::Emitter& emitter, const std::string& name, entt::meta_any& value)
	{
		emitter << YAML::Key << name;
		emitter << YAML::Value;

		return SerializeValue(emitter, value);
	}

	bool Reflection::SerializeValue(YAML::Emitter& emitter, entt::meta_any& value)
	{

		if (s_DataTypeNames.find(value.type().id()) != s_DataTypeNames.end() && s_SerializeValueFunctions.find(value.type().id()) != s_SerializeValueFunctions.end())
		{
			//Its a registered type
			//Call value serialization function
			s_SerializeValueFunctions[value.type().id()](emitter, value);
			return true;
		}
		else if (value.type().is_sequence_container())
		{
			//Its a known sequence container
			if (auto view = value.as_sequence_container(); view)
			{
				emitter << YAML::BeginSeq;

				bool result = true;

				for (auto item : view)
				{
					result = result && SerializeValue(emitter, item);
				}

				emitter << YAML::EndSeq;

				return result;
			}
			return false;
		}
		else if (value.type().is_enum())
		{
			//Its a non-registered enumeration (enum class not supported
			value.allow_cast<int32_t>();
			entt::meta_any int_any = value.cast<int32_t>();
			s_SerializeValueFunctions[int_any.type().id()](emitter, int_any);
			return true;
		}

		emitter << YAML::Null;
		return false;
	}

	entt::meta_any Reflection::DeserializeValue(YAML::Node& node)
	{
		if (!node.IsMap())
			return nullptr;
		
		auto val = node["Type"];
		if (!val)
			return nullptr;

		entt::id_type type_id = entt::resolve(StringHash::Hash(val.as<std::string>())).id();
		
		val = node["Value"];
		if (!val)
			return nullptr;

		return s_DeserializeValueFunctions[type_id](val);
	}

	bool Reflection::SerializeComponent(YAML::Emitter &emitter, entt::id_type component_type, entt::meta_handle component_handle)
	{
		auto meta = entt::resolve(component_type);
		for (auto& [id, name] : s_ComponentPropertyNames[component_type])
		{
			entt::meta_any value = component_handle->get(id);
			SerializeProperty(emitter, name, value);
		}
	}

	entt::meta_any Reflection::DeserializeComponent(YAML::Node &node)
	{
		return entt::meta_any();
	}

}