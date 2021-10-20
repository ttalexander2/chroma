#pragma once
#include <string>
#include <vector>
#include "Chroma/Utilities/GUID.h"
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/node/node.h>

namespace Chroma
{
	struct Layer
	{
		Layer(const std::string& name, int order = 0, GUID id = GUID::CreateGUID());
		Layer(const Layer&) = default;
		int Order = 0;
		std::string Name;
		bool Global = true;
		GUID ID = GUID::CreateGUID();

		inline bool operator ==(const Layer& l)
		{
			return ID == l.ID;
		}

		inline bool operator <(const Layer& l)
		{
			return Order < l.Order;
		}

		void Serialize(YAML::Emitter& out);
		void Deserialize(YAML::Node& node);
			
	};

	class World
	{
	public:
		static std::vector<Layer> Layers;

		static void Serialize(YAML::Emitter& out);
		static void Deserialize(YAML::Node& node);
	};
}