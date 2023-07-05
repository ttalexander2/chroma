#pragma once

#include <type_traits>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include "BasicHash.h"
#include "TypeData.h"
#include "Type.h"
#include "TypeHash.h"
#include "Util.h"

namespace Chroma::Reflection
{

	class MetaData;

	/**
     * @brief Type factory used for registering metadata for a type.
     * This class uses the named parameter idiom to make type registration easy to use and read.
     * @tparam T - Type associated with the type factory.
     */
    template<typename T>
    class TypeFactory
    {
        friend class TypeData;
        friend class Registry;
        template<typename Ty>
        friend class TypeFactory;
        template<typename Ty>
        friend class TypeInitializer;

        using id_hash = BasicHash<uint32_t>;

    public:
        /**
         * @brief Registers another type as a base class of this type.
         * @tparam B - Base type.
         * @return Returns a reference to this type factory.
         */
        template<typename B>
        TypeFactory<T> &base() noexcept
        {
            // Check whether the type is actually a base class.
            static_assert(!std::is_same_v<B, T> && std::is_base_of_v<B, T>, "Invalid base type.");

            TypeInfo *info = get_type_info<B>();
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
        TypeFactory<T> &conversion() noexcept
        {
            using From_ = std::remove_cv_t<std::remove_reference_t<T>>;
            using To_ = std::remove_cv_t<std::remove_reference_t<To>>;

            // Check whether the type is *actually* convertible.
            static_assert(Internal::is_convertible_v<From_, To_>,
                          "Type is not convertable.");


            TypeInfo *to_info = get_type_info<To>();

            // Register the type if it has not yet been registered (this is unlikely)
            if (to_info == nullptr)
            {
                TypeFactory<To>::register_type();
                to_info = get_type_info<To>();
            }

            // Register the conversion
            if (to_info != nullptr)
            {
                ConvInfo info{};
                info.id = to_info->id;
                info.helper_func = &Internal::convert_type<T, To>;

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
        TypeFactory<T> &conversion() noexcept
        {
            TypeInfo *to_info = get_type_info<To>();
            if (to_info == nullptr)
            {
                TypeFactory<To>::register_type();
                to_info = get_type_info<To>();
            }

            // Registers the conversion function
            if (to_info != nullptr)
            {
                ConvInfo info{};
                info.id = to_info->id;
                info.helper_func = &Internal::convert_type<T, To, Func>;

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
        TypeFactory<T> &ctor() noexcept
        {
            // Checks to make sure this type is constructible with the given arguments.
            static_assert(std::is_constructible_v<T, Args...>,
                          "Type cannot be constructed with the given arguments.");

            TypeInfo *info = _type.info();
            if (info != nullptr)
            {
                CtorInfo ctor{};
                // Since we don't require an identifier for constructor, we'll just use a counter.
                ctor.id = Internal::constructor_counter::next();
                ctor.type_id = _type.id();
                ctor.arity = sizeof...(Args);
                // Register the argument types (just in case they don't exist in the system)
                (TypeFactory<Args>::register_type(), ...);


                ctor.arg = &Internal::ctor_arg<Args...>;
                ctor.invoke = &Internal::construct<T, Args...>;

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
        TypeFactory<T> &ctor() noexcept
        {
            using descriptor = Internal::function_helper_t<T, decltype(Func)>;

            static_assert(std::is_same_v<typename descriptor::return_type, T>,
                          "Ctor func should return an object of the matching type.");

            TypeInfo *info = _type.info();
            if (info != nullptr)
            {
                CtorInfo ctor{};
                // Since we don't require an identifier for constructor, we'll just concat the typeid names for now...
                ctor.id = Internal::constructor_counter::next();
                ctor.type_id = _type.id();
                ctor.arity = descriptor::args_type::size;

                ctor.arg = &Internal::func_arg<typename descriptor::args_type>;
                ctor.invoke = &Internal::construct<T, Func>;

                info->constructors[ctor.id] = ctor;
            }

            return *this;
        }

        /**
         * @brief Registers a data member for this type.
         * @tparam Data - Pointer to the data object.
         * @param name - User provided identifier for the type.
         * @param args - Metadata to assign to the type.
         * @return Returns a reference to this type factory.
         */
        template<auto Data, typename ...Args, typename = std::enable_if_t<(std::is_base_of_v<MetaData, Args> && ...)>>
        TypeFactory &data(const std::string &name, Args&&... args)
        {
	        const uint32_t hash = BasicHash<uint32_t>::hash(name);
            TypeInfo *info = _type.info();
            if (info != nullptr)
            {
                DataInfo data;
                data.id = hash;
                data.name = name;
                data.flags = DataFlags::none;

                // Checks if the data is a member of the type.
                if constexpr (std::is_member_object_pointer_v<decltype(Data)>)
                {
                    using data_type = std::remove_reference_t<std::invoke_result_t<decltype(Data), T &>>;

                    TypeFactory<data_type>::register_type();
                    data.type_id = Internal::type_hash_v<data_type>;
                    if (std::is_const_v<data_type>)
                    {
                        data.flags |= DataFlags::is_const;
                    }
                    data.get = &Internal::get_function<T, Data>;
                    data.set = &Internal::set_function<T, Data>;
                }
                else
                {
                    using data_type = std::remove_reference_t<std::remove_pointer_t<decltype(Data)>>;

                    TypeFactory<data_type>::register_type();
                    data.type_id = Internal::type_hash_v<data_type>;
                    if (std::is_same_v<T, std::remove_const_t<data_type>> || std::is_const_v<data_type>)
                    {
                        data.flags |= DataFlags::is_const;
                    }
                    data.flags |= DataFlags::is_static;
                    data.get = &Internal::get_function<T, Data>;
                    data.set = &Internal::set_function<T, Data>;
                }

            	// Add the metadata
            	((data.metadata[Internal::type_hash_v<Args>] = args), ...);

                info->data[hash] = data;
            }

            return *this;
        }

        /**
         * @brief Registers a data member of this type using getter and setter functions.
         * @tparam Getter - Getter function, should return the data member.
         * @tparam Setter - Setter function, sets the data member. This function can be nullptr_t if the data is read-only.
         * @param name - Name of the data member.
         * @param args - MetaData to assign to the data member.
         * @return Returns a reference to this type factory.
         */
        template<auto Getter, auto Setter, typename ...Args, typename = std::enable_if_t<(std::is_base_of_v<MetaData, Args> && ...)>>
        TypeFactory &data(const std::string &name, Args&&... args)
        {
            static_assert(!std::is_same_v<decltype(Getter), std::nullptr_t>, "Getter cannot be nullptr!");
            const uint32_t hash = BasicHash<uint32_t>::hash(name);
            TypeInfo *info = _type.info();
            if (info != nullptr)
            {
                DataInfo data{};
                data.id = hash;
                data.name = name;
                data.flags = DataFlags::none;

                // If there is no setter function (i.e data is readonly)
                if constexpr (std::is_same_v<decltype(Setter), std::nullptr_t>)
                {
                    using data_type = std::invoke_result_t<decltype(Getter), T &>;

                    TypeFactory<data_type>::register_type();
                    data.type_id = Internal::type_hash_v<data_type>;
                    data.get = &Internal::get_function<T, Getter>;
                    data.set = &Internal::set_function<T, Setter>;
                    data.flags |= DataFlags::is_const;
                }
                else
                {
                    using data_type = std::invoke_result_t<decltype(Getter), T &>;

                    TypeFactory<data_type>::register_type();
                    data.type_id = Internal::type_hash_v<data_type>;
                    data.flags |= DataFlags::is_static;
                    data.get = &Internal::get_function<T, Getter>;
                    data.set = &Internal::set_function<T, Setter>;
                }

            	// Add the metadata
                ((data.metadata[Internal::type_hash_v<Args>] = args), ...);

                info->data[hash] = data;
            }

            return *this;
        }




        /**
         * @brief Registers a function to this type.
         * @tparam Func - Pointer to the function
         * @param name - User provided identifier for the function.
         * @param args - Metadata to assign to the function
         * @return Returns a reference to this type factory.
         */
        template<auto Func, typename ...Args, typename = std::enable_if_t<(std::is_base_of_v<MetaData, Args> && ...)>>
        TypeFactory &function(const std::string &name, Args&&... args)
        {
            using descriptor = Internal::function_helper_t<T, decltype(Func)>;

            uint32_t hash = BasicHash<uint32_t>::hash(name);
            TypeInfo *info = _type.info();
            if (info != nullptr)
            {
                FuncInfo func{};
                func.id = hash;
                func.name = name;
                func.arity = descriptor::args_type::size;
                func.return_type = Internal::type_hash_v<typename descriptor::return_type>;
                func.arg = &Internal::func_arg<typename descriptor::args_type>;
                func.invoke = &Internal::func_invoke<T, Func>;

            	// Add the metadata
            	((func.metadata[Internal::type_hash_v<Args>] = args), ...);

                info->functions[hash] = func;
            }

            return *this;
        }

		template <typename... Args, typename = std::enable_if_t<(std::is_base_of_v<MetaData, std::remove_pointer_t<Args>> && ...)>>
    	TypeFactory &metadata(Args&&... args)
        {
	        if (TypeInfo *info = _type.info(); info != nullptr)
        	{
        		// Add the metadata
        		((info->metadata[Internal::type_hash_v<Args>] = args), ...);
        	}

        	return *this;
        }

    private:
        /**
         * @brief Registers the type anonymously and creates a type factory.
         */
        explicit TypeFactory() : _type(Internal::TypeHash<T>::value())
        {
            register_type();
        }

        /**
         * @brief Registers the type explicitly under the given identifier.
         * @param name - Identifier for the type.
         */
        explicit TypeFactory(const std::string &name) : _type(Internal::TypeHash<T>::value())
        {
            register_type(name);
        }

        /**
         * @brief Registers a type with the reflection system.
         * @param name
         */
        static void register_type(const std::string &name)
        {
            auto id = Internal::TypeHash<T>::value();
            auto hash = BasicHash<uint32_t>::hash(name);

            // If the type does not yet exist in the reflection system.
            if (TypeData::instance().types.find(id) == TypeData::instance().types.end())
            {
                TypeInfo info;
                info.name = name;
                info.id = id;
                info.flags = TypeData::get_flags<T>();
                info.underlying_type_id = get_underlying_type();

                TypeData::instance().type_aliases[hash] = id;
                TypeData::instance().types[id] = info;
            	if constexpr (std::is_arithmetic_v<std::remove_cv_t<std::remove_reference_t<T>>>)
            	{
            		register_numeric_conversions();
            	}
            }
            else // The type already exists, just add the alias.
            {
                TypeInfo *info = get_type_info<T>();
                if (info != nullptr)
                {
                    TypeData::instance().type_aliases[hash] = id;
                    info->name = name;
                }
            }
        }

        /**
         * @brief Registers the type anonymously in the reflection system.
         */
        static void register_type()
        {
            auto id = Internal::TypeHash<T>::value();

            // Only register if the type doesn't already exist.
            if (TypeData::instance().types.find(id) == TypeData::instance().types.end())
            {
                TypeInfo info;
                info.name = typeid(Type).name();
                info.id = id;
                info.flags = TypeData::get_flags<T>();
                info.underlying_type_id = get_underlying_type();

                TypeData::instance().types[id] = info;
            	
            	if constexpr (std::is_arithmetic_v<std::remove_cv_t<std::remove_reference_t<T>>>)
            	{
            		register_numeric_conversions();
            	}
            }

        }

    	static void register_numeric_conversions()
        {
        	TypeFactory<T> factory;
        	if constexpr (!std::is_same_v<T, bool>)
        	{
        		factory = factory.conversion<bool>();
        	}
        	if constexpr (!std::is_same_v<T, float>)
        	{
        		factory = factory.conversion<float>();
        	}
        	if constexpr (!std::is_same_v<T, double>)
        	{
        		factory = factory.conversion<double>();
        	}
        	if constexpr (!std::is_same_v<T, int32_t>)
        	{
        		factory = factory.conversion<int32_t>();
        	}
        	if constexpr (!std::is_same_v<T, int64_t>)
        	{
        		factory = factory.conversion<int64_t>();
        	}
        	if constexpr (!std::is_same_v<T, uint32_t>)
        	{
        		factory = factory.conversion<uint32_t>();
        	}
        	if constexpr (!std::is_same_v<T, uint64_t>)
        	{
        		factory = factory.conversion<uint64_t>();
        	}
        	if constexpr (!std::is_same_v<T, char>)
        	{
        		factory = factory.conversion<char>();
        	}
        	if constexpr (!std::is_same_v<T, unsigned char>)
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
            if constexpr (std::is_pointer_v<T> || Internal::is_pointer_like_v<T>)
            {
                auto factory = TypeFactory<typename Internal::remove_all_pointers<T>::type>();
                return factory._type.id();
            }

            return Internal::TypeHash<void>::value();
        }

        /**
         * @brief Helper function to get the type info for this type if valid.
         * @tparam T
         * @return
         */
        template<typename TypeToGet>
        static TypeInfo *get_type_info()
        {
            Type t(Internal::TypeHash<TypeToGet>::value());
            if (t.valid())
            {
                return t.info();
            }
            return nullptr;
        }

        Type _type; // Type info for this type.
    };
}
