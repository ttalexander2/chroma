#include "chromapch.h"
#include "Asset.h"

#include "AssetManager.h"

namespace Chroma
{
	Asset::~Asset()
	{
		if (m_Loaded)
			this->Unload();
	}

	bool Asset::IsTypeOf(const TypeInfo* pTypeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(pTypeInfo);
	}

	bool Asset::IsTypeOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

}