#include "chromapch.h"
#include "Component.h"

namespace Chroma
{
	unsigned int Component::comparison_counter = 0;

	Component::type_factory Component::register_type()
	{
		return Chroma::Reflection::register_type<Component>("Component")
			.data<&Component::IsEnabled, &Component::SetEnabled>("Enabled");
	}
} //namespace Chroma
