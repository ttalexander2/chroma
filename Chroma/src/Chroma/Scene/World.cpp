#include "chromapch.h"
#include "World.h"

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

namespace Chroma
{
	std::vector<Layer> World::Layers = {Layer("Background", 0), Layer("Default", 1, GUID::Zero()), Layer("Foreground", 2), Layer("UI", 3)};

	Layer::Layer(const std::string& name, int order, GUID id)
	: Name(name), Order(order), ID(id) {}


	void Layer::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Name";
		out << YAML::Value << Name;

		out << YAML::Key << "ID";
		out << YAML::Value << ID.ToString();

		out << YAML::Key << "Order";
		out << YAML::Value << Order;
	}

	void Layer::Deserialize(YAML::Node& node)
	{
		auto val = node["Name"];
		if (val)
		{
			Name = val.as<std::string>();
		}
		val = node["ID"];
		if (val)
		{
			ID = GUID::Parse(val.as<std::string>());
		}
		val = node["Order"];
		if (val)
		{
			Order = val.as<int>();
		}

	}

	void World::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Layers" << YAML::Value << YAML::BeginSeq;
		for (auto& layer : Layers)
		{
			//CHROMA_CORE_TRACE("layer: {}", layer.Name);
			out << YAML::BeginMap;
			layer.Serialize(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
	}


	void World::Deserialize(YAML::Node& node)
	{

		auto layers = node["Layers"];
		if (layers && layers.size() > 0)
		{
			Layers.clear();
			for (auto layer : layers)
			{
				Layer l = Layer("", 0);
				l.Deserialize(layer);
				Layers.push_back(l);
			}
		}
	}
}