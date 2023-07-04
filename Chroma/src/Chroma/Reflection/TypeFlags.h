#pragma once

namespace Chroma::Reflection
{
    /**
     * @brief Flags for a type.
     */
    enum class TypeFlags : uint16_t
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
    enum class DataFlags : uint8_t
    {
        none = 0,
        is_const = 1 << 0,
        is_static = 1 << 1
    };

    // Helper Operators:

    inline TypeFlags operator|(TypeFlags a, TypeFlags b)
    {
        return static_cast<TypeFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
    }

    inline TypeFlags operator|=(TypeFlags &a, TypeFlags b)
    {
        return a = a | b;
    }

    inline TypeFlags operator&(TypeFlags a, TypeFlags b)
    {
        return static_cast<TypeFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
    }

    inline TypeFlags &operator&=(TypeFlags &a, TypeFlags b)
    {
        return a = a & b;
    }

    inline TypeFlags operator~(TypeFlags a)
    {
        return static_cast<TypeFlags>(~static_cast<uint16_t>(a));
    }

    inline TypeFlags operator^(TypeFlags a, TypeFlags b)
    {
        return static_cast<TypeFlags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
    }

    inline TypeFlags &operator^=(TypeFlags &a, TypeFlags b)
    {
        return a = a ^ b;
    }

    inline DataFlags operator|(DataFlags a, DataFlags b)
    {
        return static_cast<DataFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline DataFlags operator|=(DataFlags &a, DataFlags b)
    {
        return a = a | b;
    }

    inline DataFlags operator&(DataFlags a, DataFlags b)
    {
        return static_cast<DataFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    inline DataFlags &operator&=(DataFlags &a, DataFlags b)
    {
        return a = a & b;
    }

    inline DataFlags operator~(DataFlags a)
    {
        return static_cast<DataFlags>(~static_cast<uint8_t>(a));
    }

    inline DataFlags operator^(DataFlags a, DataFlags b)
    {
        return static_cast<DataFlags>(static_cast<uint8_t>(a) ^ static_cast<uint8_t>(b));
    }

    inline DataFlags &operator^=(DataFlags &a, DataFlags b)
    {
        return a = a ^ b;
    }

    namespace Internal
    {
        inline bool has_flag(TypeFlags val, TypeFlags flag)
        {
            return (val & flag) == flag;
        }

        inline bool has_flag(DataFlags val, DataFlags flag)
        {
            return (val & flag) == flag;
        }

        inline bool has_any_flag(TypeFlags val, TypeFlags flags)
        {
            return (val & flags) != TypeFlags::none;
        }

        inline bool has_any_flag(DataFlags val, DataFlags flags)
        {
            return (val & flags) != DataFlags::none;
        }
    }


}