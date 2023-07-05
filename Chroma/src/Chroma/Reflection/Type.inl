#pragma once

#include "Any.h"
#include "Type.h"

namespace Chroma::Reflection
{
	

	template<typename KeyType, typename>
	Any Type::user_data(KeyType &&key) const
	{
		return user_data(static_cast<uint32_t>(key));
	}

	template <typename MetaDataType, typename>
	bool Type::has_metadata() const
	{
		return has_metadata(Internal::type_hash_v<MetaDataType>);
	}

	template <typename MetaDataType, typename>
	MetaDataType Type::get_metadata() const
	{
		return *get_metadata(Internal::type_hash_v<MetaDataType>).try_cast<MetaDataType>();
	}

	

}