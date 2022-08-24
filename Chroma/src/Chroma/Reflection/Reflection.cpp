#include "chromapch.h"

#include "Chroma/Components/Collider.h"
#include "Reflection.h"

namespace Chroma
{
	std::unordered_map<uint32_t, std::function<void(YAML::Emitter &, Reflection::Any &)>> Reflection::s_SerializeYAMLFunctions;
	std::unordered_map<uint32_t, std::function<Reflection::Any(YAML::Node &)>> Reflection::s_DeserializeYAMLFunctions;
	std::unordered_map<uint32_t, std::function<void(File &, Reflection::Any &)>> Reflection::s_SerializeBinaryFunctions;
	std::unordered_map<uint32_t, std::function<Reflection::Any(File &)>> Reflection::s_DeserializeBinaryFunctions;

#define REGISTER_YAML_TYPE(type, name) RegisterYAMLSerializeFunctions<type>(    \
		[&](YAML::Emitter &emitter, Reflection::Any &value) {					\
			emitter << value.Cast<type>();										\
		},																		\
		[&](YAML::Node &node) {													\
			return node.as<type>();												\
		})

	void Reflection::InitializeDataTypes()
	{
		Reflection::Register<bool>("bool");
		Reflection::Register<float>("float");
		Reflection::Register<double>("double");
		Reflection::Register<int32_t>("int32");
		Reflection::Register<int64_t>("int64");
		Reflection::Register<uint32_t>("uint32");
		Reflection::Register<uint64_t>("uint64");
		Reflection::Register<char>("char");
		Reflection::Register<unsigned char>("uchar");
		Reflection::Register<const char *>("cstring");
		Reflection::Register<std::string>("string");

		Reflection::Register<Chroma::Ref<Chroma::Asset>>("asset");

		Reflection::Register<Math::vec2>("vec2");
		Reflection::Register<Math::vec3>("vec3");
		Reflection::Register<Math::vec4>("vec4");
		Reflection::Register<Chroma::EntityID>("entity_id");
		Reflection::Register<Chroma::GUID>("guid");

		REGISTER_YAML_TYPE(bool);
		REGISTER_YAML_TYPE(float);
		REGISTER_YAML_TYPE(double);
		REGISTER_YAML_TYPE(int32_t);
		REGISTER_YAML_TYPE(uint32_t);
		REGISTER_YAML_TYPE(int64_t);
		REGISTER_YAML_TYPE(uint64_t);
		REGISTER_YAML_TYPE(char);
		REGISTER_YAML_TYPE(unsigned char);
		REGISTER_YAML_TYPE(std::string);
		REGISTER_YAML_TYPE(Chroma::Ref<Chroma::Asset>);
		REGISTER_YAML_TYPE(Math::vec2);
		REGISTER_YAML_TYPE(Math::vec3);
		REGISTER_YAML_TYPE(Math::vec4);
		REGISTER_YAML_TYPE(Chroma::EntityID);
		REGISTER_YAML_TYPE(Chroma::GUID);
	}

	void Reflection::SerializeObjectYAML(YAML::Emitter &out, Reflection::Any object)
	{
		if (object)
		{
			CHROMA_CORE_ERROR("Could not serialize object!");
			return;
		}

		auto descriptor = Resolve<std::string>();
		//out << YAML::BeginMap << YAML::Key;
		//out << object.GetType()->GetName();
		//out << YAML::Value;
		//out << YAML::BeginMap;
		//for (Reflect::DataMember* data : object.GetType()->GetDataMembers())
		//{
		//	if (!data->GetSerialize())
		//		continue;
		//
		//	Reflect::Any value = data->Get(object);
		//	std::vector<Reflect::Conversion *> conversions = value.GetType()->GetConversions();
		//
		//
		//	size_t hash = data->GetType()->GetHash();
		//	if (s_SerializeYAMLFunctions.contains(hash)) //If the type is a registered primitive
		//	{
		//		out << YAML::Key << data->GetName() << YAML::Value;
		//		s_SerializeYAMLFunctions[hash](out, value);
		//	}
		//	else if (!conversions.empty()) //Else if there are known conversions for the type
		//	{
		//		for (Reflect::Conversion *conversion : conversions)
		//		{
		//			size_t conversion_hash = conversion->GetToType()->GetHash();
		//			//Check if this type converts to a primitive
		//			if (s_SerializeYAMLFunctions.contains(conversion_hash)) 
		//			{
		//				//Convert the value to a primitive type
		//				Reflect::Any converted_value = conversion->Convert(value.Get());
		//				out << YAML::Key << data->GetName() << YAML::Value;
		//				s_SerializeYAMLFunctions[conversion_hash](out, converted_value);
		//			}
		//		}
		//	}
		//	else if (Reflect::Resolve(value.GetType()->GetName()) != nullptr) //Else if the value is a reflectable type (non-primitive)
		//	{
		//		SerializeObjectYAML(out, value);
		//	}
		//}
		//out << YAML::EndMap;
		//out << YAML::EndMap;
	}

	void Reflection::SerliazeObjectBinary(File &file, Reflection::Any object)
	{
	}

	Reflection::Any Reflection::DeserializeObjectYAML(YAML::Node &node)
	{
		return Reflection::Any();
	}

	Reflection::Any Reflection::DeserializeObjectBinary(File &file)
	{
		return Reflection::Any();
	}

} //namespace Chroma