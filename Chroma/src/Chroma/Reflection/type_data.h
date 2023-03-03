#pragma once

#include <unordered_map>
#include <cstdint>
#include <string>
#include <vector>

#include "basic_hash.h"
#include "type_traits.h"
#include "type_flags.h"
#include "any.h"

// Note: For the current sake of simplicity, currently type data structs are stored
// within std::unordered_map (s), however I would like to make the storage for this
// type data more flexible. It would be best to provide an optional interface to the
// user to allow for custom storage and allocation of this data.

// Additionally, it would be nice in the future to optionally allow the user
// to store and provide a type_data object. This way, the user would be able to
// manage the context of the reflection system.


namespace Chroma::Reflection
{
    // Forward declarations
    struct type_info;
    struct data_info;
    struct func_info;
    struct ctor_info;
    class any;
    class handle;

    /**
     * @brief Struct storing meta info for a single piece of type data.
     */
    struct data_info
    {
        using id_type = uint32_t;
        std::string name;
        id_type id;
        id_type type_id;

        bool (*set)(handle&, any);

        any (*get)(handle);

        data_flags flags;

        std::unordered_map<uint32_t, any> user_data;
    };

    /**
     * @brief Struct storing meta info for a single type function.
     */
    struct func_info
    {
        using id_type = uint32_t;
        using size_type = std::size_t;
        std::string name;
        id_type id;

        size_type arity;
        id_type return_type;

        id_type (*arg)(const size_type) noexcept;

        any (*invoke)(handle, any *const);


        bool is_const;
        bool is_static;

        std::unordered_map<uint32_t, any> user_data;
    };

    /**
     * @brief Struct storing meta info for a single type constructor.
     */
    struct ctor_info
    {
        using id_type = uint32_t;
        using size_type = std::size_t;

        id_type id;
        id_type type_id;

        size_t arity;

        id_type (*arg)(const size_type) noexcept;

        any (*invoke)(any *const);
    };

    /**
     * @brief Struct storing meta info for a single type conversion.
     */
    struct conv_info
    {
        uint32_t id;

        any (*helper_func)(any);
    };

    /**
     * @brief Struct storing meta info for a single type.
     */
    struct type_info
    {
        std::string name;
        uint32_t id;

        type_flags flags;

        uint32_t underlying_type_id;

        std::unordered_map<uint32_t, data_info> data;
        std::unordered_map<uint32_t, func_info> functions;
        std::unordered_map<uint32_t, conv_info> conversions;
        std::unordered_map<uint32_t, ctor_info> constructors;
        std::unordered_map<uint32_t, any> user_data;
        std::vector<uint32_t> bases;
    };

    /**
     * @brief Monolithic class containing all data stored in the reflection system.
     */
    class type_data
    {
        friend class type;
        friend class type_container;
        friend class data;
        friend class data_container;
        friend class function;
        friend class function_container;
        friend class registry;
        friend class any;
        friend class argument_container;
        friend class constructor;
        friend class constructor_container;
        template<typename T>
        friend class type_factory;

        using id_hash = basic_hash<uint32_t>;

        /**
         * @brief Gets the single instance of the type_data system.
         * @return
         */
        static type_data &instance()
        {
            static type_data instance;
            return instance;
        }

        /**
         * @brief Gets the type flags for a provided type.
         * @tparam T - Type to check.
         * @return type_flags enum representing all flags for a type.
         */
        template<typename T>
        static type_flags get_flags()
        {
            type_flags flags = type_flags::none;

            if constexpr (std::is_class_v<T>)
            {
                flags |= type_flags::is_class;
            }
            if constexpr (std::is_abstract_v<T>)
            {
                flags |= type_flags::is_abstract;
            }
            if constexpr (std::is_integral_v<T>)
            {
                flags |= type_flags::is_integral;
            }
        	if constexpr (std::is_floating_point_v<T>)
        	{
        		flags |= type_flags::is_floating_point;
        	}
            if constexpr (std::is_array_v<T>)
            {
                flags |= type_flags::is_array;
            }
            if constexpr (std::is_enum_v<T>)
            {
                flags |= type_flags::is_enum;
            }
            if constexpr (std::is_pointer_v<T>)
            {
                flags |= type_flags::is_pointer;
            }
            if constexpr (internal::is_pointer_like_v<T>)
            {
                flags |= type_flags::is_pointer_like;
            }
            if constexpr (internal::is_sequence_container_v<T>)
            {
                flags |= type_flags::is_sequence_container;
            }
            if constexpr (internal::is_associative_container_v<T>)
            {
                flags |= type_flags::is_associative_container;
            }
            if constexpr (false)
            {
                flags |= type_flags::is_template_specialization;
            }

            return flags;
        }

        /**
         * @brief Map of application type info by type hash
         */
        std::unordered_map<uint32_t, type_info> types;

        /**
         * @brief Stores name to ID aliases. Key is name hash, value is type hash
         */
        std::unordered_map<uint32_t, uint32_t> type_aliases;

        /**
         * @brief Default constructor.
         */
        type_data() = default;

    };

}
