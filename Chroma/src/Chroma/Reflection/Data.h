#pragma once

#include <cstdint>
#include "Type.h"
#include "Any.h"

namespace Chroma::Reflection
{

    // Forward declarations
    class type_data;
    class Registry;
    class Any;
    class Handle;
    struct data_info;
    struct type_info;
    template<typename T>
    class type_factory;

    /**
     * @brief Class representing a field or piece of data within a type.
     * This class is a wrapper for data registered in the reflection system.
     */
    class Data
    {
        // Friend classes
        friend class type_data;
        friend class Registry;
        friend class Any;
        friend class Handle;
        friend class Type;
        friend class data_container;
        template<typename T>
        friend class type_factory;

    public:
        /**
         * @brief Creates an empty data object.
         */
        Data();

        /**
         * @brief Default copy constructor.
         */
        Data(const Data &) = default;

        /**
         * @brief Equality comparison operator.
         * @param rhs - Other data object to compare to.
         * @return Returns true if both data have the same ID and they are contained in the same type.
         */
        bool operator==(const Data &rhs) const;

        /**
         * @brief Checks whether this data object is valid (i.e. has a reference to a valid type, and the reference
         * to the information about this data exists).
         * @return Returns true if the data object is valid, false otherwise.
         */
        [[nodiscard]] bool valid() const;

        /**
         * @brief Boolean operator.
         * @return Returns true if the data object is valid.
         */
        explicit operator bool() const
        { return valid(); }

        /**
         * @brief Gets the ID of this data.
         * @return 32 bit number representing this data. (This is not the ID of the type)
         */
        [[nodiscard]] uint32_t id() const;

        /**
         * @brief Gets the name of the data.
         * @return String containing the registered name of the data.
         */
        [[nodiscard]] std::string name() const;

        /**
         * @brief The type of the data.
         * @return A type object containing the type of this data.
         */
        [[nodiscard]] Reflection::Type type() const;

        /**
         * @brief The owning type of this data.

         */
         [[nodiscard]] Reflection::Type owner() const;

        /**
         * @brief Flag representing whether this data is const.
         * @return True if the data is const, false otherwise.
         */
        [[nodiscard]] bool is_const() const;

        /**
         * @brief Flag representing whether this data is static.
         * @return True if the data is static, false otherwise.
         */
        [[nodiscard]] bool is_static() const;

        /**
         * @brief Gets the data from a given object.
         * @param handle - Object to retrieve the data.
         * @return Returns an any object containing the data.
         */
        [[nodiscard]] Any get(Reflection::Handle handle) const;

        /**
         * @brief Sets the data on a given object.
         * @param handle - Object to set the data.
         * @param value - Value to set the data to.
         * @return Returns true if the set operation was successful, false otherwise.
         */
        bool set(Reflection::Handle& handle, Any value) const;

    	/**
		 * @brief Gets a piece of user data associated with this type, from the given key.
		 * @param key - Key of the user data to retrieve.
		 * @return Any object containing the stored user data.
		 */
    	[[nodiscard]] Any user_data(const std::string& key) const;


    	/**
		 * @brief Gets a piece of user data associated with this type, from the given key.
		 * @param hash - Key of the user data to retrieve.
		 * @return Any object containing the stored user data.
		 */
    	[[nodiscard]] Any user_data(uint32_t hash) const;


    	/**
    	 * @brief Gets a piece of user data associated with the provided key.
    	 * @param key - Hashable key type.
    	 * @return Any object containing the stored user data.
    	 */
    	template <typename KeyType, typename = std::enable_if_t<std::is_same_v<std::underlying_type_t<KeyType>, uint32_t>>>
		[[nodiscard]] Any user_data(KeyType&& key) const
    	{
    		return user_data(static_cast<uint32_t>(key));
    	}

    	/**
    	 * @brief Checks whether user data contained with the key exists.
    	 * @param key - String key.
    	 */
    	[[nodiscard]] bool has_user_data(const std::string& key) const;

    	/**
    	 * @brief Checks whether user data is associated with this type under the provided key.
    	 * @param hash - Key of the user aata to retrieve.
    	 */
    	[[nodiscard]] bool has_user_data(uint32_t hash) const;

    	template <typename KeyType, typename = std::enable_if_t<std::is_same_v<std::underlying_type_t<KeyType>, uint32_t>>>
		[[nodiscard]] bool has_user_data(KeyType&& key) const
    	{
    		return has_user_data(static_cast<uint32_t>(key));
    	}

    private:
        /**
         * @brief Constructs a data object given an ID for the data, and the type of the data.
         * @param id
         * @param type_id
         */
        explicit Data(uint32_t id, uint32_t owner);

        uint32_t _id; // ID of the data.
    	uint32_t _owner; // ID of the type containing this data
    };


}