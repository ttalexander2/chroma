#pragma once
#include <string>

//Based on static reflection system from Simon Coenen's Flux Engine
//Adapted from https://simoncoenen.com/blog/programming/StaticReflection


class StringHash
{
private:


	static constexpr size_t val_const{ 0xcbf29ce484222325 };
	static constexpr size_t prime_const{ 0x100000001b3 };

	static inline constexpr size_t Hash_Internal(const char* const str, const size_t value) noexcept
	{
		return (str[0] == '\0') ? value : Hash_Internal(&str[1], (value ^ size_t(str[0])) * prime_const);
	}

public:
	static inline constexpr StringHash Hash(const char* const str) noexcept
	{
		return StringHash(Hash_Internal(str, val_const));
	}

	constexpr StringHash() noexcept
		: m_Hash(0)
	{
	}

	explicit constexpr StringHash(const size_t hash) noexcept
		: m_Hash(hash)
	{
	}

	explicit constexpr StringHash(const char* const pText) noexcept
		: m_Hash(Hash_Internal(pText, val_const))
	{
	}

	explicit StringHash(const std::string& text)
		: m_Hash(Hash_Internal(text.c_str(), val_const))
	{
	}

	inline constexpr operator size_t() const { return m_Hash; }

	inline bool operator==(const StringHash& rhs) const { return m_Hash == rhs.m_Hash; }
	inline bool operator!=(const StringHash& rhs) const { return m_Hash != rhs.m_Hash; }
	inline bool operator<(const StringHash& rhs) const { return m_Hash < rhs.m_Hash; }
	inline bool operator>(const StringHash& rhs) const { return m_Hash > rhs.m_Hash; }

	size_t m_Hash;
};


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