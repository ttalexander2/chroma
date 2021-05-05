#pragma once

#include <yaml-cpp/yaml.h>

#include "Chroma/Math/Vec2.h"
#include "Chroma/Math/Vec3.h"
#include "Chroma/Math/Vec4.h"

namespace YAML
{
	template<>
	struct convert<Math::vec2>
	{
		static Node encode(const Math::vec2& val)
		{
			Node node;
			node.push_back(val.x);
			node.push_back(val.y);
			return node;
		}

		static bool decode(const Node& node, Math::vec2& val)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			val.x = node[0].as<float>();
			val.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Math::vec3>
	{
		static Node encode(const Math::vec3& val)
		{
			Node node;
			node.push_back(val.x);
			node.push_back(val.y);
			node.push_back(val.z);
			return node;
		}

		static bool decode(const Node& node, Math::vec3& val)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			val.x = node[0].as<float>();
			val.y = node[1].as<float>();
			val.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Math::vec4>
	{
		static Node encode(const Math::vec4& val)
		{
			Node node;
			node.push_back(val.x);
			node.push_back(val.y);
			node.push_back(val.z);
			node.push_back(val.w);
			return node;
		}

		static bool decode(const Node& node, Math::vec4& val)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			val.x = node[0].as<float>();
			val.y = node[1].as<float>();
			val.z = node[2].as<float>();
			val.w = node[3].as<float>();
			return true;
		}
	};


	Emitter& operator<<(Emitter& out, const Math::vec2& v);
	Emitter& operator<<(Emitter& out, const Math::vec3& v);
	Emitter& operator<<(Emitter& out, const Math::vec4& v);
}


