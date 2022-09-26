#include "chromapch.h"
#include "Yaml.h"

namespace YAML
{
	Emitter &YAML::operator<<(Emitter &out, const c2Poly &v)
	{
		out << Flow;
		out << BeginMap << Key << "Count" << Value << v.count;

		out << Key << "Norms" << Value;
		out << BeginSeq;
		for (int i = 0; i < v.count; i++)
		{
			out << BeginSeq;
			out << v.norms[i].x;
			out << v.norms[i].y;
			out << EndSeq;
		}
		out << EndSeq;

		out << Key << "Verts" << Value;
		out << BeginSeq;
		for (int i = 0; i < v.count; i++)
		{
			out << BeginSeq;
			out << v.verts[i].x;
			out << v.verts[i].y;
			out << EndSeq;
		}
		out << EndSeq;

		out << EndMap;
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Chroma::GUID &v)
	{
		out << v.ToString();
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Chroma::EntityID &v)
	{
		out << static_cast<uint32_t>(v);
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Chroma::Ref<Chroma::Asset> &v)
	{
		out << v->GetID().ToString();
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Math::vec2 &v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << EndSeq;
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Math::vec3 &v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << EndSeq;
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Math::vec4 &v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Math::uvec2 &v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << EndSeq;
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Math::uvec3 &v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << EndSeq;
		return out;
	}

	Emitter &YAML::operator<<(Emitter &out, const Math::uvec4 &v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
		return out;
	}
}
