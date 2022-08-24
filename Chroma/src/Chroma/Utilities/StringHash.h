#pragma once
#include <string>

//Based on static reflection system from Simon Coenen's Flux Engine
//Adapted from https://simoncoenen.com/blog/programming/StaticReflection



template <typename Type, typename = std::enable_if_t<std::is_integral<Type>::value>>
class BasicHash
{
private:


	static constexpr Type val_const{ static_cast<Type>(0xcbf29ce484222325) };
	static constexpr Type prime_const{ static_cast<Type>(0x100000001b3) };

	static inline constexpr Type Hash_Internal(const char *const str, const Type value) noexcept
	{
		return (str[0] == '\0') ? value : Hash_Internal(&str[1], (value ^ Type(str[0])) * prime_const);
	}

public:
	static inline constexpr BasicHash Hash(const char *const str) noexcept
	{
		return BasicHash(Hash_Internal(str, val_const));
	}

	static inline constexpr BasicHash Hash(const std::string &str) noexcept
	{
		return BasicHash(Hash_Internal(str.c_str(), val_const));
	}

	constexpr BasicHash() noexcept
		: m_Hash(0)
	{
	}

	explicit constexpr BasicHash(const Type hash) noexcept
		: m_Hash(hash)
	{
	}

	explicit constexpr BasicHash(const char *const pText) noexcept
		: m_Hash(Hash_Internal(pText, val_const))
	{
	}

	explicit BasicHash(const std::string &text)
		: m_Hash(Hash_Internal(text.c_str(), val_const))
	{
	}

	inline constexpr operator Type() const { return m_Hash; }

	inline bool operator==(const BasicHash &rhs) const { return m_Hash == rhs.m_Hash; }
	inline bool operator!=(const BasicHash &rhs) const { return m_Hash != rhs.m_Hash; }
	inline bool operator<(const BasicHash &rhs) const { return m_Hash < rhs.m_Hash; }
	inline bool operator>(const BasicHash &rhs) const { return m_Hash > rhs.m_Hash; }

	Type m_Hash;
};

using StringHash = BasicHash<size_t>;

static constexpr size_t operator"" _hs(const char* str, std::size_t) noexcept
{
	return StringHash::Hash(str);
}

namespace std
{
	template <>
	struct hash<StringHash>
	{
		std::size_t operator()(const StringHash& hash) const
		{
			return hash.m_Hash;
		}
	};
}