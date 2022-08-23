#include "chromapch.h"
#include "Tag.h"

#include "Chroma/Reflection/Reflection.h"


namespace Chroma
{
	Reflection::TypeFactory<Tag> Chroma::Tag::RegisterType()
	{
		Reflection::Register<Tag>("Tag")
				.Base<Component>()
				.Data<&Tag::EntityName>("EntityName")
				.Data<&Tag::Ordering>("Ordering");
	}
}

