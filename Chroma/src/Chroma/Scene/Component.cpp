#include "chromapch.h"
#include "Component.h"

namespace Chroma
{

	unsigned int Component::comparison_counter = 0;

	Reflection::TypeFactory<Component> Component::RegisterType()
	{
		return Reflection::Register<Component>("Component")
				.Data<&Component::SetEnabled, &Component::IsEnabled>("Enabled");
	}

} //namespace Chroma