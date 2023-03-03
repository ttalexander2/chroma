﻿#include "chromapch.h"
#include "Reflection.h"

#include "Chroma/Assets/Asset.h"
#include "Chroma/Components/RigidBody.h"
#include "Chroma/Math/Math.h"
#include "Chroma/Scene/EntityID.h"
#include "Chroma/Utilities/Yaml.h"



namespace Chroma::Reflection
{
	
	const std::vector<uint32_t>& GetTypeRequirements(uint32_t type_id)
	{
		any type_requirements = resolve(type_id).user_data(static_cast<uint32_t>(MetaFlags::TYPE_REQUIREMENTS));
		if (type_requirements && type_requirements.can_cast<std::vector<uint32_t>>())
		{
			return *type_requirements.try_cast<std::vector<uint32_t>>();
		}
		return {};
	}

	void Serializer::SerializeObjectYAML(YAML::Emitter &out, handle object)
	{
		if (!object.type().valid())
		{
			CHROMA_CORE_ERROR("Could not serialize object!");
			return;
		}

		if (object.type().has_user_data(MetaFlags::SERIALIZE_YAML_FUNC))
		{
			auto user_data = object.type().user_data(MetaFlags::SERIALIZE_YAML_FUNC);
			const auto serialize_function_ptr = user_data.try_cast<void (*)(YAML::Emitter&, handle)>();
			
			if (serialize_function_ptr != nullptr)
			{
				const auto serialize_function = *serialize_function_ptr;
				serialize_function(out, object);
				return;
			}
		}

		for (auto d : object.type().data())
		{
			//if (!d.has_user_data(MetaFlags::SERIALIZE))
			//{
			//	continue;
			//}
			
			any value = d.get(object);

			if (d.has_user_data(MetaFlags::SERIALIZE_YAML_FUNC))
			{
				auto user_data = d.user_data(MetaFlags::SERIALIZE_YAML_FUNC);
				const auto serialize_function_ptr = user_data.try_cast<void (*)(YAML::Emitter&, handle)>();
							
				if (serialize_function_ptr != nullptr)
				{
					const auto serialize_function = *serialize_function_ptr;

					out << YAML::Key << d.name() << YAML::Value;
					serialize_function(out, object);
				}
			}
			else if (d.type().is_enum())
			{
				auto enum_type = d.type().underlying_type();
				if (enum_type.has_user_data(MetaFlags::SERIALIZE_YAML_FUNC))
				{
					auto user_data = enum_type.user_data(MetaFlags::SERIALIZE_YAML_FUNC);
					const auto serialize_function_ptr = user_data.try_cast<void (*)(YAML::Emitter&, handle)>();
							
					if (serialize_function_ptr != nullptr)
					{
						const auto serialize_function = *serialize_function_ptr;

						out << YAML::Key << d.name() << YAML::Value;
						serialize_function(out, object);
					}
				}
			}
			else if (d.type().is_sequence_container())
			{
				out << YAML::Key << d.name() << YAML::Value;
				out << YAML::BeginSeq;
				//for (auto item : value.as_sequence_container())
				//{
				//	SerializeObjectYAML(out, object);
				//}
				out << YAML::EndSeq;
			}
			else if (d.type().is_associative_container())
			{
				//CHROMA_CORE_TRACE("{}{} <-- AssociativeContainer", std::string(depth, '\t'), data.GetName());

				out << YAML::Key << d.name() << YAML::Value;
				out << YAML::BeginMap;
				//for (auto [key, item] : value.as_associative_container())
				//{
				//	out << YAML::Key;
				//	SerializeObjectYAML(out, key);
				//	out << YAML::Value;
				//	SerializeObjectYAML(out, item);
				//}
				out << YAML::EndMap;
			}
			else if (d.type().is_class())
			{
				out << YAML::Key << d.name() << YAML::Value;
				SerializeObjectYAML(out, handle(value));
			}
			else
			{
				CHROMA_CORE_WARN("{} <-- Unknown Type ({})", d.name(), d.type().name());
			}
		}
	}
	

	void Serializer::DeserializeObjectYAML(handle object, YAML::Node &node)
	{
		

	}

	void Initializer::Initialize()
	{
		
		Reflection::register_type<bool>("bool")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<bool>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<bool>);
		
		Reflection::register_type<float>("float")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<float>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<float>);
		
		Reflection::register_type<double>("double")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<double>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<double>);
		
		Reflection::register_type<int32_t>("int32")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<int32_t>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<int32_t>);
		
		Reflection::register_type<int64_t>("int64")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<int64_t>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<int64_t>);
		
		Reflection::register_type<uint32_t>("uint32")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<uint32_t>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<uint32_t>);
		
		Reflection::register_type<uint64_t>("uint64")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<uint64_t>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<uint64_t>);
		
		Reflection::register_type<char>("char")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<char>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<char>);
		
		Reflection::register_type<unsigned char>("uchar")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<unsigned char>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<unsigned char>);
		
		Reflection::register_type<const char *>("cstring");
		
		Reflection::register_type<std::string>("string")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<std::string>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<std::string>);

		Reflection::register_type<Asset>("Asset")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Ref<Asset>>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Ref<Asset>>);

		Reflection::register_type<Math::vec2>("vec2")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Math::vec2>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Math::vec2>);
		
		Reflection::register_type<Math::vec3>("vec3")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Math::vec3>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Math::vec3>);
		
		Reflection::register_type<Math::vec4>("vec4")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Math::vec4>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Math::vec4>);
		
		Reflection::register_type<Math::uvec2>("uvec2")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Math::uvec2>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Math::uvec2>);
		
		Reflection::register_type<Math::uvec3>("uvec3")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Math::uvec3>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Math::uvec3>);
		
		Reflection::register_type<Math::uvec4>("uvec4")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<Math::uvec4>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<Math::uvec4>);
		
		Reflection::register_type<EntityID>("EntityID")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<EntityID>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<EntityID>);
		
		Reflection::register_type<GUID>("GUID")
		.user_data(MetaFlags::SERIALIZE_YAML_FUNC, &Serializer::SerializeTypeYAML<GUID>)
		.user_data(MetaFlags::DESERIALIZE_YAML_FUNC, &Serializer::DeserializeTypeYAML<GUID>);

		for (auto initializer : component_initializers)
		{
			initializer();
		}
		
	}
}

