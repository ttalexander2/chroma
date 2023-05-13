#pragma once

#include <type_traits>
#include <string>

namespace Chroma::Reflection
{
    // Forward declarations
    class Type;


    /**
     * Class for string hashing to an integral value with constexpr.
     *
     * NOTE: This class is not designed to be used for security, only for use
     * in the reflection system for converting provided strings to IDs.
     *
     * @tparam T - Resulting integral hash value.
     */
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    class BasicHash
    {
    public:

        /**
         * @brief Hashes a C string.
         * @param str - String to hash.
         * @return basic_hash object containing the hash.
         */
        static constexpr BasicHash hash(const char *const str) noexcept
        {
            return BasicHash(hash_internal(str, val_const));
        }

        /**
         * @brief Hashes a std::string.
         * @param str - String to hash.
         * @return basic_hash object containing the hash.
         */
        static constexpr BasicHash hash(const std::string &str) noexcept
        {
            return BasicHash(hash_internal(str.c_str(), val_const));
        }

        /**
         * @brief Constructs a basic_hash object from an existing value.
         * @param hash - Hash value.
         */
        explicit constexpr BasicHash(const T hash) noexcept:
                m_Hash(hash)
        {
        }

        /**
         * @brief Constructs a basic_hash object from a C string.
         * @param hash - String to hash.
         */
        explicit constexpr BasicHash(const char *const pText) noexcept:
                m_Hash(hash_internal(pText, val_const))
        {
        }

        /**
         * @brief Constructs a basic_hash object from a std::string.
         * @param text - String to hash.
         */
        explicit BasicHash(const std::string &text) :
                m_Hash(hash_internal(text.c_str(), val_const))
        {
        }

        /**
         * @brief Conversion operator.
         * @return Returns the hash value as type T.
         */
        constexpr operator T() const
        { return m_Hash; }

        /**
         * @brief Comparison operator. Compares the hash value with another basic_hash's hash value.
         * @param rhs - Other hash value to compare to.
         * @return Returns true if the hash values are the same, false otherwise.
         */
        bool operator==(const BasicHash &rhs) const
        { return m_Hash == rhs.m_Hash; }

        /**
         * @brief Comparison operator. Compares the hash value with another basic_hash's hash value.
         * @param rhs - Other hash value to compare to.
         * @return Returns false if the hash values are the same, true otherwise.
         */
        bool operator!=(const BasicHash &rhs) const
        { return m_Hash != rhs.m_Hash; }

        /**
         * @brief Comparison operator. Compares the hash value with another basic_hash's hash value.
         * @param rhs - Other hash value to compare to.
         * @return Returns true if the left hash is less than the right hash value, false otherwise.
         */
        bool operator<(const BasicHash &rhs) const
        { return m_Hash < rhs.m_Hash; }

        /**
         * @brief Comparison operator. Compares the hash value with another basic_hash's hash value.
         * @param rhs - Other hash value to compare to.
         * @return Returns true if the left hash is greater than than the right hash value, false otherwise.
         */
        bool operator>(const BasicHash &rhs) const
        { return m_Hash > rhs.m_Hash; }

    private:
        // Hash constants
        static constexpr T val_const{static_cast<T>(0xcbf29ce484222325)};
        static constexpr T prime_const{static_cast<T>(0x100000001b3)};

        /**
         * @brief Internal function to perform the string hash.
         * @param str - String to hash.
         * @param value - Value constant used to perform the hash.
         * @return Resulting hash value.
         */
        static constexpr T hash_internal(const char *const str, const T value) noexcept // NOLINT(misc-no-recursion)
        {
            return (str[0] == '\0') ? value : hash_internal(&str[1], (value ^ T(str[0])) * prime_const);
        }

        // Hash value.
        T m_Hash;
    };

    /**
     * @brief 64-bit string hash.
     */
    using string_hash = BasicHash<size_t>;
}

namespace std
{
    /**
     * @brief Template specification such that string_hash can be used by std library functions.
     */
    template<>
    struct hash<Chroma::Reflection::string_hash>
    {
        std::size_t operator()(const Chroma::Reflection::string_hash &hash) const
        {
            return hash;
        }
    };
}



