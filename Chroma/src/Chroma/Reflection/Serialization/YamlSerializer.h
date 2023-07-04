#pragma once
#include "Chroma/Reflection/Any.h"
#include "Chroma/Reflection/MetaData.h"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/node/node.h"

namespace Chroma::Reflection
{
	class IYamlSerializer : public MetaData
	{
	public:
		virtual void Serialize(YAML::Emitter& emitter, Handle value) = 0;
		virtual Any Deserialize(YAML::Node& node) const = 0;
	};

		
	template <typename Type>
	class YamlSerializer final : public IYamlSerializer
	{
	public:
			
		void Serialize(YAML::Emitter& emitter, Handle value) override
		{
			if (value.can_cast_or_convert<Type>())
			{
				emitter << value.cast_or_convert<Type>();
			}
		}
			
		Any Deserialize(YAML::Node& node) const override
		{
			return Any{node.as<Type>()};
		}
	};
		
}

