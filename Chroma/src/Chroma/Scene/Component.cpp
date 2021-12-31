#include "chromapch.h"
#include "Component.h"

namespace Chroma
{

	unsigned int Component::comparison_counter = 0;

	/// @brief Function to begin the serialization process for the component.
	/// 
	/// This function begins the key/map structure for the serialization of data members.
	/// @param out YAML emitter to write to
	void Component::BeginSerialize(YAML::Emitter& out)
	{
		out << YAML::Key << GetTypeInfo()->GetTypeName();
		out << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Enabled" << YAML::Value << m_Enabled;
	}

	/// @brief Function to end the serialization process for the component.
	/// 
	/// This function ends the key/map structure for the serialization of data members.
	/// @param out YAML emitter to write to
	void Component::EndSerialize(YAML::Emitter& out)
	{
		out << YAML::EndMap;
	}

	bool Component::IsTypeOf(const TypeInfo* pTypeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(pTypeInfo);
	}

	bool Component::IsTypeOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

	void Component::DoSerialize(YAML::Emitter& out)
	{
		BeginSerialize(out);
		Serialize(out);
		EndSerialize(out);
	}

	
}