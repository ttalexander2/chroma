#pragma once
#include <Chroma/Utilities/StringHash.h>

//Adapted from https://github.com/simco50/FluxEngine/blob/master/FluxEngine/Core/TypeInfo.h

namespace Chroma
{
	class TypeInfo
	{
	public:
		TypeInfo(const char *typeName, const TypeInfo *baseTypeInfo);
		~TypeInfo() = default;

		bool IsTypeOf(const StringHash &type) const;
		bool IsTypeOf(const TypeInfo *typeInfo) const;

		template <typename T>
		bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

		const StringHash &GetType() const { return m_Type; }
		const std::string &GetTypeName() const { return m_TypeName; }
		const TypeInfo *GetBaseTypeInfo() const { return m_pBaseTypeInfo; }

	private:
		std::string m_TypeName;
		StringHash m_Type;
		const TypeInfo *m_pBaseTypeInfo = nullptr;
	};
}
