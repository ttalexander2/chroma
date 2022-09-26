#include "chromapch.h"

#include "Chroma/Components/Collider.h"
#include "Reflection.h"
#include "Chroma/Utilities/Yaml.h"

namespace Chroma
{
	std::unordered_map<uint32_t, std::function<void(YAML::Emitter &, Reflection::Any &)>> Reflection::s_SerializeYAMLFunctions;
	std::unordered_map<uint32_t, std::function<Reflection::Any(YAML::Node &)>> Reflection::s_DeserializeYAMLFunctions;
	std::unordered_map<uint32_t, std::function<void(File &, Reflection::Any &)>> Reflection::s_SerializeBinaryFunctions;
	std::unordered_map<uint32_t, std::function<Reflection::Any(File &)>> Reflection::s_DeserializeBinaryFunctions;

#define REGISTER_YAML_TYPE(type) RegisterYAMLSerializeFunctions<type>(			\
		[&](YAML::Emitter &emitter, Reflection::Any &value) {					\
			emitter << value.Cast<type>();										\
		},																		\
		[&](YAML::Node &node) {													\
			return node.as<type>();												\
		})

	void Reflection::InitializeDataTypes()
	{
		Register<bool>("bool");
		Register<float>("float");
		Register<double>("double");
		Register<int32_t>("int32");
		Register<int64_t>("int64");
		Register<uint32_t>("uint32");
		Register<uint64_t>("uint64");
		Register<char>("char");
		Register<unsigned char>("uchar");
		Register<const char *>("cstring");
		Register<std::string>("string");

		Register<Ref<Asset>>("Asset");

		Register<Math::vec2>("vec2");
		Register<Math::vec3>("vec3");
		Register<Math::vec4>("vec4");
		Register<Math::uvec2>("uvec2");
		Register<Math::uvec3>("uvec3");
		Register<Math::uvec4>("uvec4");
		Register<EntityID>("EntityID");
		Register<GUID>("GUID");

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
		REGISTER_YAML_TYPE(Math::uvec2);
		REGISTER_YAML_TYPE(Math::uvec3);
		REGISTER_YAML_TYPE(Math::uvec4);
		REGISTER_YAML_TYPE(Chroma::EntityID);
		REGISTER_YAML_TYPE(Chroma::GUID);
	}

	void Reflection::SerializeObjectYAML(YAML::Emitter &out, Any &object, int depth)
	{
		if (!object)
		{
			CHROMA_CORE_ERROR("Could not serialize object!");
			return;
		}

		//Check if the object is a primitive
		if (s_SerializeYAMLFunctions.contains(object.Type().Id())) //If the type is a registered primitive
		{
			//CHROMA_CORE_TRACE("{}{} <-- Primitive", std::string(depth, '\t'), object.Type().GetName());
			s_SerializeYAMLFunctions[object.Type().Id()](out, object);
			return;
		}

		//The object is not a primitive

		//CHROMA_CORE_INFO("{}{}", std::string(depth, '\t'), object.Type().GetName());

		//Just to prevent crashing from circular references
		if (depth >= 64)
		{
			CHROMA_CORE_ERROR("Could not serialize object! Max depth exceeded.");
			return;
		}

		depth++;

		auto descriptor = Resolve<std::string>();

		out << YAML::BeginMap;

		//Loop through data members of the object
		for (auto &data : object.Type().Data())
		{
			if (!data.Serialized(&object))
				continue;

			Any value = data.Get(object);

			uint32_t hash = data.Type().Id();
			if (s_SerializeYAMLFunctions.contains(hash)) //If the type is a registered primitive
			{
				//CHROMA_CORE_TRACE("{}{} <-- Primitive", std::string(depth, '\t'), data.GetName());
				out << YAML::Key << data.GetName() << YAML::Value;
				s_SerializeYAMLFunctions[hash](out, value);
			}
			else if (data.Type().IsEnum()) //The type is an enum.
			{
				//Find the type of the enum value
				for (auto &enumType : data.Type().Data())
				{
					if (enumType.Get({}) == value)
					{
						//CHROMA_CORE_TRACE("{}{} <-- Enum", std::string(depth, '\t'), data.GetName());
						out << YAML::Key << data.GetName() << YAML::Value << enumType.GetName();
						break;
					}
				}
			}
			else if (data.Type().IsSequenceContainer()) //The type is a sequence container
			{
				//CHROMA_CORE_TRACE("{}{} <-- SequenceContainer", std::string(depth, '\t'), data.GetName());

				out << YAML::Key << data.GetName() << YAML::Value;
				out << YAML::BeginSeq;
				for (auto item : value.AsSequenceContainer())
				{
					SerializeObjectYAML(out, object, depth);
				}
				out << YAML::EndSeq;
			}
			else if (data.Type().IsAssociativeContainer())
			{
				//CHROMA_CORE_TRACE("{}{} <-- AssociativeContainer", std::string(depth, '\t'), data.GetName());

				out << YAML::Key << data.GetName() << YAML::Value;
				out << YAML::BeginMap;
				for (auto [key, item] : value.AsAssociativeContainer())
				{
					out << YAML::Key;
					SerializeObjectYAML(out, key, depth);
					out << YAML::Value;
					SerializeObjectYAML(out, item, depth);
				}
				out << YAML::EndMap;
			}
			else if (data.Type().IsClass()) //Else if the value is a reflectable type (non-primitive)
			{
				out << YAML::Key << data.GetName() << YAML::Value;
				SerializeObjectYAML(out, value, depth);
			}
			else
			{
				CHROMA_CORE_WARN("{}{} <-- Unknown Type ({})", std::string(depth, '\t'), data.GetName(), data.Type().GetName());
			}
		}
		out << YAML::EndMap;

		depth--;
	}

	void Reflection::SerliazeObjectBinary(File &file, Any &object, int depth)
	{
	}

	void Reflection::DeserializeObjectYAML(Any &any, Type type, YAML::Node &node, int depth)
	{
		if (!any.Valid())
		{
			CHROMA_CORE_ERROR("Could not deserialize object! DEPTH: {}", depth);
			return;
		}

		//CHROMA_CORE_TRACE("{}", object.Type().GetName());

		//Just to prevent crashing from circular references
		if (depth >= 64)
		{
			CHROMA_CORE_ERROR("Could not serialize object! Max depth exceeded.");
			return;
		}

		depth++;
		for (auto &data : type.Data())
		{
			if (data.Serialized() && node[data.GetName()])
			{
				CHROMA_CORE_TRACE("{}", data.GetName());

				auto data_node = node[data.GetName()];

				if (s_DeserializeYAMLFunctions.contains(data.Type().Id()))
				{
					Any val = s_DeserializeYAMLFunctions[data.Type().Id()](data_node);
					if (data.GetName() == "Color")
					{
						auto color = any.Get(data.Id()).Cast<Math::vec4>();
						CHROMA_CORE_TRACE("Color: {}, {}, {}, {}", color.r, color.g, color.b, color.a);
					}
					any.Set(data.Id(), val);
					if (data.GetName() == "Color")
					{
						auto color = any.Get(data.Id()).Cast<Math::vec4>();
						CHROMA_CORE_TRACE("Color: {}, {}, {}, {}", color.r, color.g, color.b, color.a);
					}
				}
				else if (data.Type().IsEnum())
				{
					for (auto &enumType : data.Type().Data())
					{
						if (data_node)
						{
							if (enumType.GetName() == data_node.as<std::string>())
							{
								Any val = enumType.Get({});
								any.Set(data.Id(), val);
								break;
							}
						}
					}
				}
				else if (data.Type().IsSequenceContainer())
				{
					if (data_node && data_node.IsSequence())
					{
						Any containerAny = data.Get(any.Handle());

						SequenceContainer container = containerAny.AsSequenceContainer();
						auto valType = container.ValueType();

						for (int i = 0; i < data_node.size(); i++)
						{
							YAML::Node item = data_node[i];
							Any val = valType.Construct();
							DeserializeObjectYAML(val, valType, item);
							container[i] = val;
						}
						any.Set(data.Id(), container);
					}
				}
				else if (data.Type().IsAssociativeContainer())
				{
					if (data_node && data_node.IsMap())
					{
						Any val = data.Get(any.Handle());
						AssociativeContainer container = val.AsAssociativeContainer();
						auto keyType = container.KeyType();
						auto valType = container.ValueType();

						for (int i = 0; i < data_node.size(); i++)
						{
							YAML::Node keyNode = data_node[i];
							YAML::Node valueNode = data_node[i][keyNode.as<std::string>()];

							Any key = keyType.Construct();
							DeserializeObjectYAML(key, keyType, keyNode);
							Any value = valType.Construct();
							DeserializeObjectYAML(value, valType, valueNode);

							container.Insert(key, value);
						}

						any.Set(data.Id(), container);
					}
				}
				else if (data.Type().IsClass())
				{
					Any val = data.Get(any.Handle());
					DeserializeObjectYAML(val, data.Type(), data_node);
					any.Set(data.Id(), val);
				}
				else
				{
					CHROMA_CORE_WARN("{}{} <-- Unknown Type ({})", std::string(depth, '\t'), data.GetName(), data.Type().GetName());
				}
			}
		}
		depth--;

		//*any = object; 
	}

	void Reflection::DeserializeObjectBinary(Any &any, Type type, File &file, int depth)
	{
	}
} //namespace Chroma
