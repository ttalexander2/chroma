#include "chromapch.h"
#include "CSharpScript.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	Reflection::TypeFactory<CSharpScript> CSharpScript::RegisterType()
	{
		return Reflection::Register<CSharpScript>("CSharpScript")
				.Base<Component>()
				.Data<&CSharpScript::ModuleName>("Module");
	}

} //namespace Chroma