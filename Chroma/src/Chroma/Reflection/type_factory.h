#pragma once

#include <type_traits>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <functional>
#include "basic_hash.h"
#include "type_data.h"
#include "type.h"
#include "type_hash.h"
#include "util.h"

namespace Chroma::Reflection
{

    /**
     * @brief Type factory used for registering metadata for a type.
     * This class uses the named parameter idiom to make type registration easy to use and read.
     * @tparam Type - Type associated with the type factory.
     */
    template<typename Type>
    class type_factory
    {
        friend class type_data;
        friend class registry;
        template<typename T>
        friend class type_factory;
        template<typename T>
        friend class type_initializer;

        using id_hash = basic_hash<uint32_t>;

    public:
        /**
         * @brief Registers another type as a base class of this type.
         * @tparam B - Base type.
         * @return Returns a reference to this type factory.
         */
        template<typename B>
        type_factory<Type> &base() noexcept
        {
            // Check whether the type is actually a base class.
            static_assert(!std::is_same_v<B, Type> && std::is_base_of_v<B, Type>, "Invalid base type.");

            type_info *info = get_type_info<B>();
            if (info != nullptr)
            {
                info->bases.push_back(info->id);
            }
            return *this;
        }

        /**
         * @brief Registers this type as convertible to another type.
         * @tparam To - Type which this type is convertible to.
         * @return Returns a reference to this type factory.
         */
        template<typename To>
        type_factory<Type> &conversion() noexcept
        {
            using From_ = std::remove_cv_t<std::remove_reference_t<Type>>;
            using To_ = std::remove_cv_t<std::remove_reference_t<To>>;

            // Check whether the type is *actually* convertible.
            static_assert(internal::is_convertible_v<From_, To_>,
                          "Type is not convertable.");


            type_info *to_info = get_type_info<To>();

            // Register the type if it has not yet been registered (this is unlikely)
            if (to_info == nullptr)
            {
                type_factory<To>::register_type();
                to_info = get_type_info<To>();
            }

            // Register the conversion
            if (to_info != nullptr)
            {
                conv_info info{};
                info.id = to_info->id;
                info.helper_func = &internal::convert_type<Type, To>;

                if (_type.valid() && _type.info())
                {
                    _type.info()->conversions[info.id] = info;
                }
            }

            return *this;
        }

        /**
         * @brief Registers this type as convertible to another type with the provided conversion function.
         * @tparam To - Type which this type is convertible to.
         * @tparam Func - Conversion function. Function should take an instance of this type, and return an instance of the other type (To).
         * @return Returns a reference to this type factory.
         */
        template<typename To, auto Func>
        type_factory<Type> &conversion() noexcept
        {
            type_info *to_info = get_type_info<To>();
            if (to_info == nullptr)
            {
                type_factory<To>::register_type();
                to_info = get_type_info<To>();
            }

            // Registers the conversion function
            if (to_info != nullptr)
            {
                conv_info info{};
                info.id = to_info->id;
                info.helper_func = &internal::convert_type<Type, To, Func>;

                if (_type.valid() && _type.info())
                {
                    _type.info()->conversions[info.id] = info;
                }
            }

            return *this;
        }

        /**
         * @brief Registers a constructor with the given arguments.
         * @tparam Args - Argument types for the constructor.
         * @return Returns a reference to this type factory.
         */
        template<typename... Args>
        type_factory<Type> &ctor() noexcept
        {
            // Checks to make sure this type is constructible with the given arguments.
            static_assert(std::is_constructible_v<Type, Args...>,
                          "Type cannot be constructed with the given arguments.");

            type_info *info = _type.info();
            if (info != nullptr)
            {
                ctor_info ctor{};
                // Since we don't require an identifier for constructor, we'll just use a counter.
                ctor.id = internal::constructor_counter::next();
                ctor.type_id = _type.id();
                ctor.arity = sizeof...(Args);
                // Register the argument types (just in case they don't exist in the system)
                (type_factory<Args>::register_type(), ...);


                ctor.arg = &internal::ctor_arg<Args...>;
                ctor.invoke = &internal::construct<Type, Args...>;

                info->constructors[ctor.id] = ctor;
            }

            return *this;
        }

        /**
         * @brief Registers a constructor function for this type.
         * @tparam Func - Function which produces an instance of this type.
         * @return Returns a reference to this type factory.
         */
        template<auto Func>
        type_factory<Type> &ctor() noexcept
        {
            using descriptor = internal::function_helper_t<Type, decltype(Func)>;

            static_assert(std::is_same_v<typename descriptor::return_type, Type>,
                          "Ctor func should return an object of the matching type.");

            type_info *info = _type.info();
            if (info != nullptr)
            {
                ctor_info ctor{};
                // Since we don't require an identifier for constructor, we'll just concat the typeid names for now...
                ctor.id = internal::constructor_counter::next();
                ctor.type_id = _type.id();
                ctor.arity = descriptor::args_type::size;

                ctor.arg = &internal::func_arg<typename descriptor::args_type>;
                ctor.invoke = &internal::construct<Type, Func>;

                info->constructors[ctor.id] = ctor;
            }

            return *this;
        }

        /**
         * @brief Registers a data member for this type.
         * @tparam Data - Pointer to the data object.
         * @param name - User provided identifier for the type.
         * @param user_data - Custom data to store with the data.
         * @return Returns a reference to this type factory.
         */
        template<auto Data>
        type_factory &data(const std::string &name, std::vector<std::tuple<uint32_t, any>> user_data = {})
        {
	        const uint32_t hash = basic_hash<uint32_t>::hash(name);
            type_info *info = _type.info();
            if (info != nullptr)
            {
                data_info data;
                data.id = hash;
                data.name = name;
                data.flags = data_flags::none;

                // Checks if the data is a member of the type.
                if constexpr (std::is_member_object_pointer_v<decltype(Data)>)
                {
                    using data_type = std::remove_reference_t<std::invoke_result_t<decltype(Data), Type &>>;

                    type_factory<data_type>::register_type();
                    data.type_id = internal::type_hash_v<data_type>;
                    if (std::is_const_v<data_type>)
                    {
                        data.flags |= data_flags::is_const;
                    }
                    data.get = &internal::get_function<Type, Data>;
                    data.set = &internal::set_function<Type, Data>;
                }
                else
                {
                    using data_type = std::remove_reference_t<std::remove_pointer_t<decltype(Data)>>;

                    type_factory<data_type>::register_type();
                    data.type_id = internal::type_hash_v<data_type>;
                    if (std::is_same_v<Type, std::remove_const_t<data_type>> || std::is_const_v<data_type>)
                    {
                        data.flags |= data_flags::is_const;
                    }
                    data.flags |= data_flags::is_static;
                    data.get = &internal::get_function<Type, Data>;
                    data.set = &internal::set_function<Type, Data>;
                }

                for (auto item : user_data)
                {
                    data.user_data[std::get<0>(item)] = std::get<1>(item);
                }

                info->data[hash] = data;
            }

            return *this;
        }

        /**
         * @brief Registers a data member of this type using getter and setter functions.
         * @tparam Getter - Getter function, should return the data member.
         * @tparam Setter - Setter function, sets the data member. This function can be nullptr_t if the data is read-only.
         * @param name - Name of the data member.
         * @param serialize - Whether the data member should be serialized.
         * @return Returns a reference to this type factory.
         */
        template<auto Getter, auto Setter>
        type_factory &data(const std::string &name, const std::vector<std::tuple<uint32_t, any>>& user_data = {})
        {
            static_assert(!std::is_same_v<decltype(Getter), std::nullptr_t>, "Getter cannot be nullptr!");
            const uint32_t hash = basic_hash<uint32_t>::hash(name);
            type_info *info = _type.info();
            if (info != nullptr)
            {
                data_info data{};
                data.id = hash;
                data.name = name;
                data.flags = data_flags::none;

                // If there is no setter function (i.e data is readonly)
                if constexpr (std::is_same_v<decltype(Setter), std::nullptr_t>)
                {
                    using data_type = std::invoke_result_t<decltype(Getter), Type &>;

                    type_factory<data_type>::register_type();
                    data.type_id = internal::type_hash_v<data_type>;
                    data.get = &internal::get_function<Type, Getter>;
                    data.set = &internal::set_function<Type, Setter>;
                    data.flags |= data_flags::is_const;
                }
                else
                {
                    using data_type = std::invoke_result_t<decltype(Getter), Type &>;

                    type_factory<data_type>::register_type();
                    data.type_id = internal::type_hash_v<data_type>;
                    data.flags |= data_flags::is_static;
                    data.get = &internal::get_function<Type, Getter>;
                    data.set = &internal::set_function<Type, Setter>;
                }

                for (auto item : user_data)
                {
                    data.user_data[std::get<0>(item)] = std::get<1>(item);
                }

                info->data[hash] = data;
            }

            return *this;
        }


        template<auto Data>
        type_factory &data(const std::string &name, const std::vector<std::tuple<std::string, any>>& user_data)
        {
            std::vector<std::tuple<uint32_t, any>> user_data_hashed;
            user_data_hashed.reserve(user_data.size());

            for (auto& item : user_data)
            {
                user_data_hashed.emplace_back(basic_hash<uint32_t>::hash(std::get<0>(item)), std::get<1>(item));
            }
            return data<Data>(name, user_data_hashed);
        }

        template<auto Getter, auto Setter>
        type_factory &data(const std::string &name, const std::vector<std::tuple<std::string, any>>& user_data)
        {
            std::vector<std::tuple<uint32_t, any>> user_data_hashed;
            user_data_hashed.reserve(user_data.size());

            for (auto& item : user_data)
            {
                user_data_hashed.emplace_back(basic_hash<uint32_t>::hash(std::get<0>(item)), std::get<1>(item));
            }
            return data<Getter, Setter>(name, user_data_hashed);
        }




        /**
         * @brief Registers a function to this type.
         * @tparam Func - Pointer to the function
         * @param name - User provided identifier for the function.
         * @return Returns a reference to this type factory.
         */
        template<auto Func>
        type_factory &function(const std::string &name, const std::vector<std::tuple<uint32_t, any>>& user_data = {})
        {
            using descriptor = internal::function_helper_t<Type, decltype(Func)>;

            uint32_t hash = basic_hash<uint32_t>::hash(name);
            type_info *info = _type.info();
            if (info != nullptr)
            {
                func_info func{};
                func.id = hash;
                func.name = name;
                func.arity = descriptor::args_type::size;
                func.return_type = internal::type_hash_v<typename descriptor::return_type>;
                func.arg = &internal::func_arg<typename descriptor::args_type>;
                func.invoke = &internal::func_invoke<Type, Func>;

                for (auto item : user_data)
                {
                    func.user_data[std::get<0>(item)] = std::get<1>(item);
                }

                info->functions[hash] = func;
            }

            return *this;
        }

        /**
         * @brief Registers a function to this type.
         * @tparam Func - Function pointer
         * @param name - User provided identifier for the function.
         * @param user_data - List of key value pairs for user data.
         * @return Returns a reference to this type factory.
         */
        template<auto Func>
        type_factory &function(const std::string &name, const std::vector<std::tuple<std::string, any>>& user_data)
        {
            std::vector<std::tuple<uint32_t, any>> user_data_hashed;
            user_data_hashed.reserve(user_data.size());

            for (auto& item : user_data)
            {
                user_data_hashed.emplace_back(basic_hash<uint32_t>::hash(std::get<0>(item)), std::get<1>(item));
            }
            return function<Func>(name, user_data_hashed);
        }

    	type_factory &user_data(uint32_t key, any value)
        {
        	type_info *info = _type.info();
        	if (info != nullptr)
        	{
        		info->user_data[key] = std::move(value);
        	}

        	return *this;
        }

    	template <typename KeyType, typename = std::enable_if_t<std::is_same_v<std::underlying_type_t<KeyType>, uint32_t>>>
		type_factory &user_data(KeyType&& key, any value)
    	{
        	return user_data(static_cast<uint32_t>(key), value);
    	}

        /**
         * @brief Associates key/value user data to this type.
         * @param key - Key of the data
         * @param value - Value of the data
         * @return Returns a reference to the type factory.
         */
        type_factory &user_data(const std::string& key, any value)
        {
            return user_data(basic_hash<uint32_t>::hash(key), value);
        }



        type_factory &user_data(const std::tuple<uint32_t, any>& data)
        {
            return user_data(std::get<0>(data), std::get<1>(data));
        }

        type_factory &user_data(const std::tuple<std::string, any>& data)
        {
            return user_data(basic_hash<uint32_t>::hash(std::get<0>(data)), std::get<1>(data));
        }

    private:
        /**
         * @brief Registers the type anonymously and creates a type factory.
         */
        explicit type_factory() : _type(internal::type_hash<Type>::value())
        {
            register_type();
        }

        /**
         * @brief Registers the type explicitly under the given identifier.
         * @param name - Identifier for the type.
         */
        explicit type_factory(const std::string &name) : _type(internal::type_hash<Type>::value())
        {
            register_type(name);
        }

        /**
         * @brief Registers a type with the reflection system.
         * @param name
         */
        static void register_type(const std::string &name)
        {
            auto id = internal::type_hash<Type>::value();
            auto hash = basic_hash<uint32_t>::hash(name);

            // If the type does not yet exist in the reflection system.
            if (type_data::instance().types.find(id) == type_data::instance().types.end())
            {
                type_info info;
                info.name = name;
                info.id = id;
                info.flags = type_data::get_flags<Type>();
                info.underlying_type_id = get_underlying_type();

                type_data::instance().type_aliases[hash] = id;
                type_data::instance().types[id] = info;
            	if constexpr (std::is_arithmetic_v<std::remove_cv_t<std::remove_reference_t<Type>>>)
            	{
            		register_numeric_conversions();
            	}
            }
            else // The type already exists, just add the alias.
            {
                type_info *info = get_type_info<Type>();
                if (info != nullptr)
                {
                    type_data::instance().type_aliases[hash] = id;
                    info->name = name;
                }
            }
        }

        /**
         * @brief Registers the type anonymously in the reflection system.
         */
        static void register_type()
        {
            auto id = internal::type_hash<Type>::value();

            // Only register if the type doesn't already exist.
            if (type_data::instance().types.find(id) == type_data::instance().types.end())
            {
                type_info info;
                info.name = typeid(Type).name();
                info.id = id;
                info.flags = type_data::get_flags<Type>();
                info.underlying_type_id = get_underlying_type();

                type_data::instance().types[id] = info;
            	
            	if constexpr (std::is_arithmetic_v<std::remove_cv_t<std::remove_reference_t<Type>>>)
            	{
            		register_numeric_conversions();
            	}
            }

        }

    	static void register_numeric_conversions()
        {
        	type_factory<Type> factory;
        	if constexpr (!std::is_same_v<Type, bool>)
        	{
        		factory = factory.conversion<bool>();
        	}
        	if constexpr (!std::is_same_v<Type, float>)
        	{
        		factory = factory.conversion<float>();
        	}
        	if constexpr (!std::is_same_v<Type, double>)
        	{
        		factory = factory.conversion<double>();
        	}
        	if constexpr (!std::is_same_v<Type, int32_t>)
        	{
        		factory = factory.conversion<int32_t>();
        	}
        	if constexpr (!std::is_same_v<Type, int64_t>)
        	{
        		factory = factory.conversion<int64_t>();
        	}
        	if constexpr (!std::is_same_v<Type, uint32_t>)
        	{
        		factory = factory.conversion<uint32_t>();
        	}
        	if constexpr (!std::is_same_v<Type, uint64_t>)
        	{
        		factory = factory.conversion<uint64_t>();
        	}
        	if constexpr (!std::is_same_v<Type, char>)
        	{
        		factory = factory.conversion<char>();
        	}
        	if constexpr (!std::is_same_v<Type, unsigned char>)
        	{
        		factory = factory.conversion<unsigned char>();
        	}
        	
        }

        /**
         * @brief Helper function to get the underlying type where applicable.
         * @return The ID of the underlying type, if valid. Returns 0xFFFFFFFF if invalid.
         */
        static uint32_t get_underlying_type()
        {
            if constexpr (std::is_pointer_v<Type> || internal::is_pointer_like_v<Type>)
            {
                auto factory = type_factory<typename internal::remove_all_pointers<Type>::type>();
                return factory._type.id();
            }

            return internal::type_hash<void>::value();
        }

        /**
         * @brief Helper function to get the type info for this type if valid.
         * @tparam T
         * @return
         */
        template<typename T>
        static type_info *get_type_info()
        {
            type t(internal::type_hash<T>::value());
            if (t.valid())
            {
                return t.info();
            }
            return nullptr;
        }

        type _type; // Type info for this type.
    };
}
