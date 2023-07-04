#include "chromapch.h"
#include "Reflection.h"

#include "Chroma/Assets/Asset.h"
#include "Chroma/Components/RigidBody.h"
#include "Chroma/Math/Math.h"
#include "Chroma/Scene/EntityID.h"
#include "Chroma/Utilities/Yaml.h"
#include "Serialization/YamlSerializer.h"



namespace Chroma::Reflection
{

	void Initializer::Initialize()
	{
		
		Reflection::register_type<bool>("bool")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<bool>()));
		
		Reflection::register_type<float>("float")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<float>()));
		
		Reflection::register_type<double>("double")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<double>()));
		
		Reflection::register_type<int32_t>("int32")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<int32_t>()));
		
		Reflection::register_type<int64_t>("int64")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<int64_t>()));
		
		Reflection::register_type<uint32_t>("uint32")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<uint32_t>()));
		
		Reflection::register_type<uint64_t>("uint64")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<uint64_t>()));
		
		Reflection::register_type<char>("char")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<char>()));
		
		Reflection::register_type<unsigned char>("uchar")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<unsigned char>()));
		
		Reflection::register_type<const char *>("cstring");
		
		Reflection::register_type<std::string>("string")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<std::string>()));

		Reflection::register_type<Asset>("Asset")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Ref<Asset>>()));

		Reflection::register_type<Math::vec2>("vec2")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Math::vec2>()));
		
		Reflection::register_type<Math::vec3>("vec3")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Math::vec3>()));
		
		Reflection::register_type<Math::vec4>("vec4")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Math::vec4>()));
		
		Reflection::register_type<Math::uvec2>("uvec2")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Math::uvec2>()));
		
		Reflection::register_type<Math::uvec3>("uvec3")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Math::uvec3>()));
		
		Reflection::register_type<Math::uvec4>("uvec4")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<Math::uvec4>()));
		
		Reflection::register_type<EntityID>("EntityID")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<EntityID>()));
		
		Reflection::register_type<GUID>("GUID")
		.metadata(static_cast<IYamlSerializer*>(new YamlSerializer<GUID>()));

		for (auto initializer : component_initializers)
		{
			initializer();
		}
		
	}
}

