#pragma once


#include "Chroma/IO/File.h"
#include "Chroma/Scene/ComponentRegistry.h"

#include "yaml-cpp/emitter.h"
#include "yaml-cpp/node/node.h"


#include <mirr/mirr.h>

namespace Chroma
{
	// Forward Declarations
	struct File;
	class Application;
	
	namespace Reflection
	{
		using namespace mirr;

		enum class MetaFlags : uint32_t
		{
			SERIALIZE = 0,
			TYPE_REQUIREMENTS = 1 << 0,
			SERIALIZE_YAML_FUNC = 2 << 0,
			DESERIALIZE_YAML_FUNC = 3 << 0,
			SERIALIZE_BINARY_FUNC = 4 << 0,
			DESERIALIZE_BINARY_FUNC = 5 << 0
		};
	
		const std::vector<uint32_t>& GetTypeRequirements(uint32_t type_id);



		template <typename Type, typename = std::enable_if_t<std::is_base_of_v<Component, Type> && !std::is_abstract_v<Type>>>
		type_factory<Type> RegisterComponent(const std::string& name)
		{
			auto factory = register_type<Type>(name);
			ComponentRegistry::RegisterComponent<Type>(name);
			return factory;
		}

		class Serializer
		{
			template<typename>
			friend class type_initializer;
			friend class Application;
			friend class Initializer;
		public:


			static void SerializeObjectYAML(YAML::Emitter &out, handle object);
			static void DeserializeObjectYAML(handle object, YAML::Node &node);

		private:

			template <typename Type>
			static void SerializeTypeYAML(YAML::Emitter& emitter, any value)
			{
				if (value.can_cast_or_convert<Type>())
				{
					emitter << value.cast_or_convert<Type>();
				}
			}

			template <typename Type>
			static any DeserializeTypeYAML(YAML::Node& node)
			{
				return any{node.as<Type>()};
			}
		};

		class Initializer
		{
			template <typename T> friend class ComponentInitializer;
		public:
			static void Initialize();
			
			template <typename Type, typename = std::enable_if_t<std::is_base_of_v<Component, Type> || std::is_same_v<Component, Type>>>
			static void AddComponentInitializer()
			{
				std::function func = []()
				{
					Type::register_type();
				};
				component_initializers.push_back(func);
			}

		private:
			static inline std::vector<std::function<void()>> component_initializers{};
		};
		
	}



}


