#include "chromapch.h"
#include "CSharpScript.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	Reflection::TypeFactory<CSharpScript> CSharpScript::register_type()
	{
		return Reflection::RegisterComponent<CSharpScript>("CSharpScript")
		       .base<Component>()
		       .data<&CSharpScript::ModuleName>("Module");
	}
} //namespace Chroma
