#include "chromapch.h"
#include "Serializer.h"

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

#include "Chroma/Reflection/Reflection.h"
#include "Chroma/Reflection/Serialization/YamlSerializer.h"


namespace Chroma::Reflection
{
	void Serializer::SerializeObjectToYaml(YAML::Emitter &out, Handle object)
	{
		if (!object.type().valid())
		{
			CHROMA_CORE_ERROR("Could not serialize object!");
			return;
		}
	
		CHROMA_CORE_TRACE("Serializing: {}", object.type().name());
	
		if (object.type().has_metadata<IYamlSerializer*>())
		{
			if (const auto serializer = object.type().get_metadata<IYamlSerializer*>())
			{
				serializer->Serialize(out, object);
			}
			else
			{
				out << 0;
			}
		}
		else if (object.type().is_enum())
		{
			const auto enum_type = object.type().underlying_type();
			if (enum_type.has_metadata<IYamlSerializer*>())
			{
				if (const auto serializer = enum_type.get_metadata<IYamlSerializer*>())
				{
					serializer->Serialize(out, object);
				}
				else
				{
					out << 0;
				}
			}
			else
			{
				out << 0;
			}
		}
		else if (object.type().is_sequence_container())
		{
			CHROMA_CORE_TRACE("{} <-- Sequence Container", object.type().name());
			
			out << YAML::BeginSeq;
			//for (auto item : value.as_sequence_container())
			//{
			//	SerializeObjectYAML(out, object);
			//}
			out << "I'm a sequence :)";
			out << YAML::EndSeq;
		}
		else if (object.type().is_associative_container())
		{
			CHROMA_CORE_TRACE("{} <-- Associative Container", object.type().name());
			
			out << YAML::BeginMap;
			//for (auto [key, item] : value.as_associative_container())
			//{
			//	out << YAML::Key;
			//	SerializeObjectYAML(out, key);
			//	out << YAML::Value;
			//	SerializeObjectYAML(out, item);
			//}
			out << YAML::Key << "Key" << YAML::Value << "Ima map";
			out << YAML::EndMap;
		}
		else if (object.type().is_class())
		{
	
			out << YAML::BeginMap;
			for (auto d : object.type().data())
			{
				auto dataObj = d.get(object);
				out << YAML::Key << d.name() << YAML::Value;
				SerializeObjectToYaml(out, Handle(dataObj));
			}
			
			out << YAML::EndMap;
			
		}
		else
		{
			CHROMA_CORE_WARN("{} <-- Unknown Type ({})", object.type().name(), object.type().name());
		}
	}

	void Serializer::DeserializeObjectFromYaml(Handle object, YAML::Node &node)
	{
		if (!object.type().valid())
		{
			CHROMA_CORE_ERROR("Could not deserialize object!");
			return;
		}

		CHROMA_CORE_TRACE("Deserializing: {}", object.type().name());

		if (object.type().has_metadata<IYamlSerializer*>())
		{
			if (const auto serializer = object.type().get_metadata<IYamlSerializer*>())
			{
				object.assign(serializer->Deserialize(node));
			}
		}
		else if (object.type().is_enum())
		{
			const auto enum_type = object.type().underlying_type();
			if (enum_type.has_metadata<IYamlSerializer*>())
			{
				if (const auto serializer = enum_type.get_metadata<IYamlSerializer*>())
				{
					object.assign(serializer->Deserialize(node));
				}
			}
		}
		else if (object.type().is_sequence_container())
		{
			CHROMA_CORE_TRACE("{} <-- Sequence Container", object.type().name());
			
		}
		else if (object.type().is_associative_container())
		{
			CHROMA_CORE_TRACE("{} <-- Associative Container", object.type().name());
			
		}
		else if (object.type().is_class())
		{
			for (auto d : object.type().data())
			{
				auto value = node[d.name().c_str()];
				if (value)
				{
					auto dataAny = d.get(object);
					DeserializeObjectFromYaml(Handle(dataAny), value);
					d.set(object, dataAny);
				}
			}
		}
		else
		{
			CHROMA_CORE_WARN("{} <-- Unknown Type ({})", object.type().name(), object.type().name());
		}
	}
}
