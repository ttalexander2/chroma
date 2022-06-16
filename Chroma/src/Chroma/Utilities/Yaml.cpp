#include "chromapch.h"
#include "Yaml.h"

namespace YAML
{

	Emitter& YAML::operator<<(Emitter& out, const Chroma::GUID& v) 
	{
		out << v.ToString();
		return out;
	}

	Emitter& YAML::operator<<(Emitter& out, const Chroma::EntityID& v)
	{
		out << (uint32_t)v;
		return out;
	}

	Emitter& YAML::operator<<(Emitter& out, const Chroma::Ref<Chroma::Asset>& v) 
	{
		out << v->GetID().ToString();
		return out;
	}

	Emitter& YAML::operator<<(Emitter& out, const Math::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& YAML::operator<<(Emitter& out, const Math::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	Emitter& YAML::operator<<(Emitter& out, const Math::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
}


