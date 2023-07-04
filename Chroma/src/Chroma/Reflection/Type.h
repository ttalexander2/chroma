#pragma once

#include "MetaData.h"

#include <cstdint>
#include <string>
#include <vector>
#include "TypeFlags.h"
#include "TypeHash.h"

namespace Chroma::Reflection
{

    // Forward declarations
    class TypeData;
    class func_data;
    class Registry;
    class Any;
    class Handle;
    struct TypeInfo;
    class Data;
    class Function;
    class Constructor;
    class data_container;
    class function_container;
    class constructor_container;
	
    template<typename T>
    class TypeFactory;

    /**
     * @brief Class representing meta information about a type registered with the reflection system.
     * This class is a wrapper for data stored within the reflection system.
     */
    class Type
    {
        friend class TypeData;
        friend class func_data;
        friend class Registry;
        friend class Any;
        friend class Handle;
        friend class Data;
        friend class Function;
        friend class type_container;
        friend class argument_container;
        friend class Constructor;
        friend class constructor_container;
        template<typename T>
        friend class TypeFactory;

    public:
        /**
         * @brief Creates an empty type object.
         */
        Type();

        /**
         * @brief Default copy constructor.
         */
        Type(const Type &) = default;
        
        ~Type() = default;

        /**
         * @brief Equality comparison operator.
         * @param rhs - Other type object to compare to.
         * @return Returns true if the two types are the same, false otherwise.
         */
        inline bool operator==(const Type &rhs) const
        { return id() == rhs.id(); }

        /**
         * @brief Checks whether this type object is valid (i.e. the type is registered in the reflection system)
         * @return Returns true if the type object is valid, false otherwise.
         */
        [[nodiscard]] bool valid() const;

        /**
         * @brief Boolean operator.
         * @return Returns true if the type object is valid, false otherwise.
         */
        explicit operator bool() const
        { return valid(); }

        /**
         * @brief Gets the id of the type.
         * @return 32-bit identifier associated with the type.
         */
        [[nodiscard]] uint32_t id() const;

        /**
         * @brief Gets the name associated with the type.
         * @return Name of the type.
         */
        [[nodiscard]] std::string name() const;

        /**
         * @brief Gets a hash of the type.
         * @return 32bit hash.
         */
        [[nodiscard]] uint32_t hash() const;

        /**
         * @brief Checks whether the type is an integral type.
         * @return Returns true if the type is an integral, false otherwise.
         */
        [[nodiscard]] bool is_integral() const;

    	/**
 		* @brief Checks whether the type is a number.
 		* @return Returns true if the type is numeric, false otherwise.
 		*/
    	[[nodiscard]] bool is_floating_point() const;


    	/**
		 * @brief Checks whether the type is a number.
		 * @return Returns true if the type is numeric, false otherwise.
		 */
    	[[nodiscard]] bool is_arithmetic() const;

        /**
         * @brief Checks whether the type is an array.
         * @return Returns true if the type is an array, false otherwise.
         */
        [[nodiscard]] bool is_array() const;

        /**
         * @brief Checks whether the type is an enum.
         * @return Returns true if the type is an enum, false otherwise.
         */
        [[nodiscard]] bool is_enum() const;

        /**
         * @brief Checks whether the type is a class.
         * @return Returns true if the type is a class, false otherwise.
         */
        [[nodiscard]] bool is_class() const;

        /**
         * @brief Checks whether the type is a pointer.
         * @return Returns true if the type is a pointer, false otherwise.
         */
        [[nodiscard]] bool is_pointer() const;

        /**
         * @brief Checks whether the type is pointer-like (pointer, std::shared_ptr, or std::unique_ptr)
         * @return Returns true if the type is pointer-like, false otherwise.
         */
        [[nodiscard]] bool is_pointer_like() const;

        /**
         * @brief Checks whether the type is a sequence container.
         * @return Returns true if the type is a sequence container, false otherwise.
         */
        [[nodiscard]] bool is_sequence_container() const;

        /**
         * @brief Checks whether the type is an associative container.
         * @return Returns true if the type is an associative container, false otherwise.
         */
        [[nodiscard]] bool is_associative_container() const;

        /**
         * @brief Checks whether the type is a template specialization.
         * @return Returns true if the type is a template specialization, false otherwise.
         */
        [[nodiscard]] bool is_template_specialization() const;

        /**
         * @brief Checks whether the type is abstract.
         * @return Returns true if the type is abstract, false otherwise.
         */
        [[nodiscard]] bool is_abstract() const;

        /**
         * @brief Gets the underlying type, where applicable.
         * @return Returns the underlying type if the type has an underlying type, otherwise returns an invalid type object.
         */
        [[nodiscard]] Type underlying_type() const;
        
        /**
         * @brief Gets a constructor for this type, from the given constructor ID.
         * @param id - ID of the constructor.
         * @return Constructor object associated with a constructor for this type.
         */
        [[nodiscard]] Reflection::Constructor constructor(uint32_t id) const;

        /**
         * @brief Gets an iterable container representing all registered constructors for this type.
         * @return constructor_container associated with this type.
         */
        [[nodiscard]] Reflection::constructor_container constructor() const;

        /**
         * @brief Gets data from this type associated with the given name.
         * @param name - Name of the data.
         * @return Returns a data object. The data object will be valid if no data under the given name could be found.
         */
        [[nodiscard]] Reflection::Data data(const std::string &name) const;

        /**
         * @brief Gets data from this type associated with the given ID.
         * @param id - ID of the data.
         * @return Returns a data object.
         */
        [[nodiscard]] Reflection::Data data(uint32_t id) const;

        /**
         * @brief Gets an iterable container providing all data members of this type.
         * @return data_container object associated with this type.
         */
        [[nodiscard]] Reflection::data_container data() const;

        /**
         * @brief Gets a function associated with this type from the given name.
         * @param name - Name of the function.
         * @return Function object associated with the type and function.
         */
        [[nodiscard]] Reflection::Function func(const std::string &name) const;

        /**
         * @brief Gets a function associated with this type from the given ID.
         * @param id - ID of the function.
         * @return Function object associated with the type and function.
         */
        [[nodiscard]] Reflection::Function func(uint32_t id) const;

        /**
         * @brief Gets an iterable container of all functions associated with this type.
         * @return function_container object associated with this type.
         */
        [[nodiscard]] Reflection::function_container func() const;
    	

        /**
         * @brief Checks whether the type is convertible to the specified type.
         * @param type_id - ID of the type to check for conversion.
         * @return Returns true if the type is convertible, false otherwise.
         */
        [[nodiscard]] bool is_convertible(uint32_t type_id) const;

        /**
         * @brief Checks whether the type is convertible to the specified type.
         * @tparam To - Type to check for conversion to.
         * @return Returns true if the type is convertible, false otherwise.
         */
        template<typename To>
        [[nodiscard]] bool is_convertible() const
        {
            return is_convertible(Internal::type_hash_v<To>);
        }

    	template<typename T>
    	[[nodiscard]] bool is()
        {
	        return id() == Internal::type_hash_v<T>;
        }

    	template<typename T>
    	[[nodiscard]] bool is() const
        {
	        return id() == Internal::type_hash_v<T>;
        }

    	[[nodiscard]] std::vector<uint32_t> bases() const;

        /**
         * @brief Gets the flags for this type. This enum represents all the flags for a type,
         * which can either be checked using this enum, or any of the is_... functions.
         * @return Enum holding all of the flags of this type.
         */
        [[nodiscard]] TypeFlags flags() const;




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

    	template <typename KeyType, typename = std::enable_if_t<std::is_same_v<std::underlying_type_t<KeyType>, uint32_t>>>
		[[nodiscard]] Any user_data(KeyType&& key) const;


    	[[nodiscard]] bool has_user_data(const std::string& key) const;
    	
    	[[nodiscard]] bool has_user_data(uint32_t hash) const;

    	template <typename KeyType, typename = std::enable_if_t<std::is_same_v<std::underlying_type_t<KeyType>, uint32_t>>>
		[[nodiscard]] bool has_user_data(KeyType&& key) const
    	{
    		return has_user_data(static_cast<uint32_t>(key));
    	}

		[[nodiscard]] bool has_metadata(uint32_t type_id) const;

    	template <typename MetaDataType, typename = std::enable_if_t<std::is_base_of_v<MetaData, std::remove_pointer_t<MetaDataType>>>>
    	[[nodiscard]] bool has_metadata() const;

    	[[nodiscard]] Any get_metadata(uint32_t type_id) const;

    	template <typename MetaDataType, typename = std::enable_if_t<std::is_base_of_v<MetaData, std::remove_pointer_t<MetaDataType>>>>
    	[[nodiscard]] MetaDataType get_metadata() const;

    private:
        [[nodiscard]] TypeInfo *info() const;


    private:
        explicit Type(TypeId id);

        TypeId _id;

    };

}