#include "chromapch.h"
#include "Tag.h"



namespace Chroma
{
	Reflection::TypeFactory<Tag> Tag::register_type()
	{
		return Reflection::RegisterComponent<Tag>("Tag")
		       .base<Component>()
		       .data<&Tag::EntityName>("EntityName")
		       .data<&Tag::Ordering>("Ordering");
	}
}
