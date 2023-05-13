#pragma once

namespace Chroma::Reflection
{
    /**
     * @brief Flags for a type.
     */
    enum class type_flags : uint16_t
    {
        none = 0,
        is_class = 1 << 0,
        is_abstract = 1 << 1,
        is_integral = 1 << 2,
    	is_floating_point = 1 << 3,
        is_array = 1 << 4,
        is_enum = 1 << 5,
        is_pointer = 1 << 6,
        is_pointer_like = 1 << 7,
        is_sequence_container = 1 << 8,
        is_associative_container = 1 << 9,
        is_template_specialization = 1 << 10
    };

    /**
     * @brief Flags for a type's data.
     */
    enum class data_flags : uint8_t
    {
        none = 0,
        is_const = 1 << 0,
        is_static = 1 << 1
    };

    // Helper Operators:

    inline type_flags operator|(type_flags a, type_flags b)
    {
        return static_cast<type_flags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
    }

    inline type_flags operator|=(type_flags &a, type_flags b)
    {
        return a = a | b;
    }

    inline type_flags operator&(type_flags a, type_flags b)
    {
        return static_cast<type_flags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
    }

    inline type_flags &operator&=(type_flags &a, type_flags b)
    {
        return a = a & b;
    }

    inline type_flags operator~(type_flags a)
    {
        return static_cast<type_flags>(~static_cast<uint16_t>(a));
    }

    inline type_flags operator^(type_flags a, type_flags b)
    {
        return static_cast<type_flags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
    }

    inline type_flags &operator^=(type_flags &a, type_flags b)
    {
        return a = a ^ b;
    }

    inline data_flags operator|(data_flags a, data_flags b)
    {
        return static_cast<data_flags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline data_flags operator|=(data_flags &a, data_flags b)
    {
        return a = a | b;
    }

    inline data_flags operator&(data_flags a, data_flags b)
    {
        return static_cast<data_flags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    inline data_flags &operator&=(data_flags &a, data_flags b)
    {
        return a = a & b;
    }

    inline data_flags operator~(data_flags a)
    {
        return static_cast<data_flags>(~static_cast<uint8_t>(a));
    }

    inline data_flags operator^(data_flags a, data_flags b)
    {
        return static_cast<data_flags>(static_cast<uint8_t>(a) ^ static_cast<uint8_t>(b));
    }

    inline data_flags &operator^=(data_flags &a, data_flags b)
    {
        return a = a ^ b;
    }

    namespace Internal
    {
        inline bool has_flag(type_flags val, type_flags flag)
        {
            return (val & flag) == flag;
        }

        inline bool has_flag(data_flags val, data_flags flag)
        {
            return (val & flag) == flag;
        }

        inline bool has_any_flag(type_flags val, type_flags flags)
        {
            return (val & flags) != type_flags::none;
        }

        inline bool has_any_flag(data_flags val, data_flags flags)
        {
            return (val & flags) != data_flags::none;
        }
    }


}