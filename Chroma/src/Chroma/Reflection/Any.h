#pragma once


#include <utility>
#include <iostream>
#include "Type.h"
#include "type_hash.h"
#include "type_traits.h"


namespace Chroma::Reflection
{

    // Forward declarations
    class Any;
    class Handle;


    namespace Internal
    {
        /**
         * @brief Internal operations to perform using the vtable.
         */
        enum class AnyOperation : uint8_t
        {
            copy,
            move,
            transfer,
            assign,
            destroy,
            compare,
            get,
            ref
        };

        /**
         * @brief Policy for any objects.
         */
        enum class AnyPolicy : uint8_t
        {
            owner,
            ref,
            cref
        };
    }


    /**
     * @brief Opaque container for storing data of any type.
     */
    class Any
    {
        friend class Handle;

        // Defines length and alignment for in situ storage.
        static constexpr std::size_t Len = sizeof(double[2]);
        static constexpr std::size_t Align = alignof(std::aligned_storage_t<Len + !Len>);

        using storage_type = std::aligned_storage_t<Len + !Len, Align>;
        using vtable_type = const void *(const Internal::AnyOperation, const Any &, const void *);

        template<typename Type>
        static constexpr bool in_situ = Len && alignof(Type) <= alignof(storage_type) &&
                                        sizeof(Type) <= sizeof(storage_type) &&
                                        std::is_nothrow_move_constructible_v<Type>;

    public:

        /**
         * @brief Constructs an empty any object.
         */
        Any() noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
                : instance{},
                  type_info{Internal::type_hash<void>::value()},
                  policy{Internal::AnyPolicy::owner},
                  vtable{}
        {}

        /**
         * @brief Constructs an any object of the given type and value.
         * @tparam - Type Type of the object to store.
         * @param - value Value to store.
         */
        template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, Any>>>
        Any(Type &&value) // NOLINT(google-explicit-constructor)
                :Any{}
        {
            initialize<std::decay_t<Type>>(std::forward<Type>(value));
        }

        Any(const Any& other, const Internal::AnyPolicy policy) noexcept
        : instance{other.data()},
        type_info{other.type_info},
        vtable{other.vtable},
        policy{policy}

        {

        }

        /**
         * @brief Constructs an any object  of a given Type with optional arguments.
         * @tparam Type - Type of the object to construct.
         * @tparam Args - Type of the arguments to pass to the constructor.
         * @param args - Arguments to pass to the constructor.
         */
        template<typename Type, typename... Args>
        explicit Any(std::in_place_type_t<Type>, Args &&... args)
                :Any{}
        {
            initialize<Type>(std::forward<Args>(args)...);
        }

        /**
         * @brief Copy constructor. Copies the value from another any object to this object.
         * @param other - Other any to copy to this object.
         */
        Any(const Any &other)
                : Any{}
        {
            if (other.vtable)
            {
                other.vtable(Internal::AnyOperation::copy, other, this);
            }
        }

        /**
         * @brief Move constructor. Moves the value and metadata from the other object to this.
         * @param other - Object to move.
         */
        Any(const Any &&other) noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
                : instance{},
                  type_info{other.type_info},
                  vtable{other.vtable},
                  policy{other.policy}
        {
            if (other.vtable)
            {
                other.vtable(Internal::AnyOperation::move, other, this);
            }
        }

        /**
         * @brief Deconstructor. Destroys the any object. Will only destroy the value if this any
         * object is the owner of the value.
         */
        ~Any()
        {
            if (vtable && owner())
            {
                vtable(Internal::AnyOperation::destroy, *this, nullptr);
            }
        }

        /**
         * @brief Assignment operator. Copies the value from another any object.
         * @param other - Object to assign value from.
         * @return Reference to the any object.
         */
        Any &operator=(const Any &other) noexcept // NOLINT(bugprone-unhandled-self-assignment)
        {
            reset();
            if (other.vtable)
            {
                other.vtable(Internal::AnyOperation::copy, other, this);
            }
            return *this;
        }

        /**
         * @brief Move operator. Moves a value from one any to another.
         * @param - other Any object to move.
         * @return Reference to the any object.
         */
        Any &operator=(Any &&other) noexcept // NOLINT(bugprone-unhandled-self-assignment)
        {
            reset();
            if (other.vtable)
            {
                other.vtable(Internal::AnyOperation::move, other, this);
                type_info = other.type_info;
                vtable = other.vtable;
                policy = other.policy;
            }
            return *this;
        }

        /**
         * @brief Assignment operator. Assigns this object's value from a value.
         * @tparam Type - Type of the value to assign.
         * @param value - Value to assign.
         */
        template<typename Type>
        std::enable_if_t<!std::is_same_v<std::decay_t<Type>, Any>, Any &> // NOLINT(misc-unconventional-assign-operator)
        operator=(Type &&value)
        {
            emplace<std::decay_t<Type>>(std::forward<Type>(value));
            return *this;
        }

        /**
         * @brief Gets a const raw pointer to the data stored in the any object.
         * @return Const pointer to the data.
         */
        [[nodiscard]] const void *data() const noexcept
        {
            return vtable ? vtable(Internal::AnyOperation::get, *this, nullptr) : nullptr;
        }

        /**
         * @brief Gets a const raw pointer to the data stored in the any object.
         * @param info - ID of the type contained in the any object.
         * @return Returns a const pointer to the data if the provided type matches the type of data
         * stored, else returns nullptr.
         */
        [[nodiscard]] const void *data(type_id info) const noexcept
        {
            return type_info._id == info ? data() : nullptr;
        }

        /**
         * @brief Gets a raw pointer to the data stored in the any object.
         * @return Pointer to the data.
         */
        [[nodiscard]] void *data() noexcept
        {
            return (!vtable || policy == Internal::AnyPolicy::cref) ? nullptr
                                                                     : const_cast<void *>(vtable(
                            Internal::AnyOperation::get, *this, nullptr));
        }

        /**
         * @brief Gets a raw pointer to the data stored in the any object.
         * @param info -  ID of the type contained in the any object.
         * @return Returns a pointer to the data if the provided type matches the type of data
         * stored, else returns nullptr.
         */
        [[nodiscard]] void *data(type_id info) noexcept
        {
            return type_info._id == info ? data() : nullptr;
        }


        /**
         * @brief Gets the type of the value stored in the any object.
         * @return Type object containing information about the type.
         */
        [[nodiscard]] const Type &type() const noexcept
        {
            return type_info;
        }

        /**
         * @brief Get whether the value is owned by the any object.
         * @return True if this any object owns the value, false otherwise.
         */
        [[nodiscard]] bool owner() const noexcept
        {
            return policy == Internal::AnyPolicy::owner;
        }

        /**
         * @brief Resets this any object, destroys the value (if owned), and resets type information.
         */
        void reset()
        {
            if (vtable && owner())
            {
                vtable(Internal::AnyOperation::destroy, *this, nullptr);
            }

            type_info._id = Internal::type_hash_v<void>;
            vtable = nullptr;
            policy = Internal::AnyPolicy::owner;
        }

        /**
         * @brief Resets the object and assigns a new value.
         * @tparam Type - Type of the object to assign.
         * @tparam Args - Type of arguments.
         * @param args - Arguments used to construct an object of the Type.
         */
        template<typename Type, typename... Args>
        void emplace(Args &&...args)
        {
            reset();
            initialize<Type>(std::forward<Args>(args)...);
        }

        /**
         * @brief Resets this object, and assigns a new value using an existing any object.
         * @param other - Other any object used to assign.
         * @return Returns true if the assign operation was successful, false otherwise.
         */
        bool assign(const Any &other)
        {
            if (vtable && policy != Internal::AnyPolicy::cref && type_info == other.type_info)
            {
                return (vtable(Internal::AnyOperation::assign, *this, other.data()) != nullptr);
            }
            return false;
        }

        /**
         * @brief Bool operator.
         * @return Returns true if this object is valid, false otherwise.
         */
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return vtable != nullptr;
        }

        /**
         * @brief Comparison operator.
         * @param other - Other any object to compare to.
         * @return Returns true if the object contain the same type and values, false otherwise.
         */
        bool operator==(const Any &other) const noexcept
        {
            if (vtable && type_info == other.type_info)
            {
                return (vtable(Internal::AnyOperation::compare, *this, other.data()) != nullptr);
            }
            return (!vtable && !other.vtable);
        }

    	/**
    	 * @brief Checks if the data is of type Type.
    	 * @tparam Type - Type to check.
    	 * @return True if the type is Type, false otherwise.
    	 */
    	template<typename Type>
    	[[nodiscard]] bool is_type() const noexcept
        {
	        return type_info.id() == Internal::type_hash_v<Type>;
        }

        /**
         * @brief Tries to cast this object to the provided type.
         * @tparam Type - Type to cast this object to.
         * @return Returns a pointer of the Type if the conversion was successful, returns nullptr otherwise.
         */
        template<typename Type>
        Type *try_cast() noexcept
        {
            auto id = Internal::type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value();
            if (id == type_info._id)
            {
                Type* data = static_cast<Type *>(this->data(id));
                if (data != nullptr)
                    return data;
            }
            return nullptr;
        }

    	template<typename Type>
		const Type *try_cast() const noexcept
        {
        	auto id = Internal::type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value();
        	if (id == type_info._id)
        	{
        		const Type* data = static_cast<const Type *>(this->data(id));
        		if (data != nullptr)
        			return data;
        	}
        	return nullptr;
        }

        /**
         * @brief Tries to cast this object to the provided type, or converts the any object to the type if
         * the object is convertible.
         *
         * If conversion is possible, the any object is converted, and type information for this object will reflect
         * the converted type.
         *
         * @tparam Type - Type of the object to attempt cast or conversion.
         * @return Pointer to the casted or converted type if the conversion was successful, otherwise returns nullptr.
         */
        template<typename Type>
        Type *try_cast_or_convert() noexcept
        {
            auto id = Internal::type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value();
            if (id == type_info._id)
            {
                return static_cast<Type *>(this->data(id));
            }

        	if (type_info.is_convertible<Type>())
            {
                Any result = try_conversion(id);
                reset();
                if (result.vtable)
                {
                    result.vtable(Internal::AnyOperation::move, result, this);
                    type_info = result.type_info;
                    vtable = result.vtable;
                    policy = result.policy;
                }
                return try_cast<Type>();
            }
            return nullptr;
        }

        /**
         * @brief Converts the any object to the provided type if the object is convertible.
         * @tparam Type - Type to convert the value to.
         * @return Any object with the type converted to the new type.
         */
        template<typename Type>
        Any convert() noexcept
        {
            auto id = Internal::type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value();
        	if (type_info.is_convertible<Type>())
            {
                return try_conversion(id);
            }
            return Any{};
        }

        /**
         * @brief Casts this object to the provided type.
         *
         * Note: "try_cast" is recommended over this function, as this function is unsafe.
         * @tparam Type - Type of the value.
         * @tparam Ret - Return type.
         * @return Value casted to the provided type.
         */
        template<typename Type, typename Ret = std::remove_cv_t<std::remove_reference_t<Type>>>
        Ret cast() const noexcept
        {
        	return *try_cast<Ret>();
        }

    	template<typename Type, typename Ret = std::remove_cv_t<std::remove_reference_t<Type>>>
    	Ret cast() noexcept
        {
        	return *try_cast<Ret>();
        }

        /**
         * @brief Casts this object to the provided type, or converts the any object to the type if
         * the object is convertible.
         *
         * If conversion is possible, the any object is converted, and type information for this object will reflect
         * the converted type.
         *
         * Note: "try_cast_or_convert" is recommended over this function, as this function is unsafe.
         *
         * @tparam Type - Type of the object to attempt cast or conversion.
         * @tparam Ret - Return type.
         * @return Pointer to the casted or converted type if the conversion was successful, otherwise returns nullptr.
         */
        template<typename Type, typename Ret = std::remove_cv_t<std::remove_reference_t<Type>>>
        Ret cast_or_convert() noexcept
        {
            if (can_convert<Ret>())
            {
                Any val = convert<Ret>();
                return *val.try_cast<Ret>();
            }
            return *try_cast<Ret>();
        }

        /**
         * @brief Checks whether this object can be casted to the provided type.
         * @tparam Type - Type to check for cast.
         * @return Returns true if the object can be cast, false otherwise.
         */
        template<typename Type>
        [[nodiscard]] bool can_cast() noexcept
        {
            return try_cast<std::remove_cv_t<std::remove_reference_t<Type>>>() != nullptr;
        }

        /**
         * @brief Checks whether this object can be converted to the provided type.
         *
         * @tparam Type - Type to check for conversion.
         * @return Returns true of the object can be converted, false otherwise.
         */
        template<typename Type>
    	[[nodiscard]] bool can_convert() const noexcept
        {
            return type_info.is_convertible<Type>();
        }

        /**
         * @brief Checks whether the object can bbe cast or converted to the provided type.
         * @tparam Type - Type to check.
         * @return Returns true if the object can be cast or converted to, false otherwise.
         */
        template<typename Type>
        [[nodiscard]] bool can_cast_or_convert()
        {
            return can_cast<Type>() || can_convert<Type>();
        }

    private:

        /**
         * @brief Initializes the any object for a given type.
         * Constructs type information, vtable for operations, allocates storage if necessary, and initializes the
         * value from the given arguments.
         * @tparam Type - Type of value this any object will store.
         * @tparam Args - Type of arguments.
         * @param args - Arguments passed to the value constructor for initialization.
         */
        template<typename Type, typename... Args>
        void initialize([[maybe_unused]] Args &&... args)
        {
            if constexpr (!std::is_void_v<Type>)
            {
                type_info._id = Internal::type_hash_v<Type>;
                vtable = basic_vtable<std::remove_cv_t<std::remove_reference_t<Type>>>;

                if constexpr (std::is_lvalue_reference_v<Type>)
                {
                    instance = (std::addressof(args), ...);
                    policy = std::is_const_v<std::remove_reference_t<Type>> ?
                             Internal::AnyPolicy::cref : Internal::AnyPolicy::ref;
                } else if constexpr (in_situ<Type>)
                {
                    if constexpr (sizeof...(Args) != 0u && std::is_aggregate_v<Type>)
                    {
                        new(&storage) Type{std::forward<Args>(args)...};
                    } else
                    {
                        new(&storage) Type(std::forward<Args>(args)...);
                    }
                } else
                {
                    if constexpr (sizeof...(Args) != 0u && std::is_aggregate_v<Type>)
                    {
                        instance = new Type{std::forward<Args>(args)...};
                    } else if constexpr (!std::is_abstract<Type>())
                    {
                        instance = new Type(std::forward<Args>(args)...);
                    }
                }
            }
        }


        /**
         * @brief VTable containing operations needed for any type, to be created on initialization.
         * @tparam Type - Type of the value stored in the any object.
         * @param op - Operation to perform.
         * @param value - Value to pass to the operation.
         * @param other - Any other data needed for performing the operation.
         * @return Any data returned by the operation.
         */
        template<typename Type>
        static const void *basic_vtable(const Internal::AnyOperation op, const Any &value, const void *other)
        {
            static_assert(
                    !std::is_void_v<Type> && std::is_same_v<std::remove_cv_t<std::remove_reference_t<Type>>, Type>,
                    "Invalid type!");
            const Type *val;

            // Checks whether the Type is stored "in situ",
            // AKA stored inline without the need for additional allocation.
            if constexpr (in_situ<Type>)
            {
                val = value.owner() ? reinterpret_cast<const Type *>(&value.storage)
                                    : static_cast<const Type *>(value.instance);
            } else
            {
                val = static_cast<const Type *>(value.instance);
            }

            switch (op)
            {
                case Internal::AnyOperation::copy:
                    if constexpr (std::is_copy_constructible_v<Type>)
                    {
                        static_cast<Any *>(const_cast<void *>(other))->initialize<Type>(*val);
                    }
                    break;
                case Internal::AnyOperation::move:
                    if constexpr (in_situ<Type>)
                    {
                        if (value.owner())
                        {
                            return new(&static_cast<Any *>(const_cast<void *>(other))->storage)
                                    Type{std::move(*const_cast<Type *>(val))};
                        }
                    }
                    return (static_cast<Any *>(const_cast<void *>(other))->instance =
                                    std::exchange(const_cast<Any &>(value).instance, nullptr));
                case Internal::AnyOperation::transfer:
                    if constexpr (std::is_move_assignable_v<Type>)
                    {
                        *const_cast<Type *>(val) = std::move(*static_cast<Type *>(const_cast<void *>(other)));
                        return other;
                    }
                    [[fallthrough]];
                case Internal::AnyOperation::assign:
                    if constexpr (std::is_copy_assignable_v<Type>)
                    {
                        *const_cast<Type *>(val) = *static_cast<const Type *>(other);
                        return other;
                    }
                    break;
                case Internal::AnyOperation::destroy:
                    if constexpr (in_situ<Type>)
                    {
                        val->~Type();
                    } else if constexpr (std::is_array_v<Type>)
                    {
                        delete[] val;
                    } else
                    {
                        delete val;
                    }
                    break;
                case Internal::AnyOperation::compare:
                    if constexpr (!std::is_function_v<Type> && !std::is_array_v<Type> &&
                                  Internal::is_equality_comparable_v<Type>)
                    {
                        return *val == *static_cast<const Type * >(other) ? other : nullptr;
                    } else
                    {
                        return (val == other) ? other : nullptr;
                    }
                case Internal::AnyOperation::get:
                    return val;
            }

            return nullptr;
        }

        /**
         * @brief Converts an object to another type given the type id.
         * @param type_id - ID of the type to convert to.
         * @return Converted any object.
         */
        Any try_conversion(uint32_t type_id);

    private:
        union
        {
            const void *instance{};
            storage_type storage;
        };
        Reflection::Type type_info{};
        Internal::AnyPolicy policy = Internal::AnyPolicy::owner;
        vtable_type *vtable;
    };

    class Handle : public Any
    {
    public:
    	
	    /**
         * @brief Creates a handle for some value from an lvalue reference.
         * @tparam Type 
         * @param value 
         */
        template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, Any>>>
        explicit Handle(Type &value)
        {
            initialize<Type&>(std::forward<Type&>(value));
        }

	    // ReSharper disable once CppParameterMayBeConstPtrOrRef
	    explicit Handle(Any& value) : Any(value, Internal::AnyPolicy::ref)
        {
        	type_info = value.type_info;
        }
    };

}