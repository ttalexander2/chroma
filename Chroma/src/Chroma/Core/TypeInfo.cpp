#include "chromapch.h"
#include "TypeInfo.h"

namespace Chroma
{
	TypeInfo::TypeInfo(const char *typeName, const TypeInfo *baseTypeInfo) :
		m_TypeName(typeName),
		m_Type(typeName),
		m_pBaseTypeInfo(baseTypeInfo)
	{
	}


	bool TypeInfo::IsTypeOf(const StringHash &type) const
	{
		auto pInfo = this;
		while (pInfo != nullptr)
		{
			if (type == pInfo->m_Type)
			{
				return true;
			}
			pInfo = pInfo->m_pBaseTypeInfo;
		}
		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo *typeInfo) const
	{
		if (typeInfo == nullptr)
		{
			CHROMA_CORE_ASSERT(false, "TypeInfo: Attempted reflection type was nullptr!");
			return false;
		}
		return IsTypeOf(typeInfo->GetType());
	}
}
