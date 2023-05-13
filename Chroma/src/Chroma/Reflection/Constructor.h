#pragma once

#include <cstdint>
#include "Type.h"
#include "Any.h"

namespace Chroma::Reflection
{

    // Forward Declarations
    class type_data;
    class Registry;
    struct ctor_info;
    struct type_info;
    class constructor_container;
    class argument_container;
    template<typename T>
    class type_factory;


    /**
     * @brief Class representing a constructor for a specific type.
     * This class is a wrapper for data registered in the reflection system.
     */
    class Constructor
    {
        // Friend classes
        friend class type_data;
        friend class Registry;
        friend class Any;
        friend class Handle;
        friend class Type;
        friend class argument_container;
        friend class constructor_container;
        template<typename T>
        friend class type_factory;

    public:
        /**
         * @brief Creates an empty constructor object.
         */
        Constructor();

        /**
         * @brief Default copy constructor.
         */
        Constructor(const Constructor &) = default;

        /**
         * @brief Equality comparison operator. Compares two constructors.
         * @param rhs - Other constructor to compare to.
         * @return Returns true if the two constructors are the same, false otherwise.
         */
        bool operator==(const Constructor &rhs) const;

        /**
         * @brief Whether this constructor object is valid (i.e. has a reference to a valid type, and the reference to
         * data about this constructor exists).
         * @return Returns true if the constructor object is valid, false otherwise.
         */
        [[nodiscard]] bool valid() const;

        /**
         * @brief Boolean operator.
         * @return Returns true if the constructor object is valid.
         */
        explicit operator bool() const
        { return valid(); }

        /**
         * @brief Gets the ID of this constructor.
         * @return 32 bit number representing this constructor. (This is not the ID of the type)
         */
        [[nodiscard]] uint32_t id() const;

        /**
         * @brief Gets the type this constructor constructs.
         * @return Type object containing information about the type.
         */
        [[nodiscard]] Reflection::Type type() const;

        /**
         * @brief Gets the arity of this constructor.
         * @return Number of arguments this constructor takes.
         */
        [[nodiscard]] size_t arity() const;

        /**
         * @brief Gets the type of an argument at an index.
         * @param index - Index of the argument to get.
         * @return Type object representing the type of the argument. If the constructor or index is invalid,
         * the type will be void.
         */
        [[nodiscard]] Reflection::Type args(size_t index) const;

        /**
         * @brief Invokes the constructor with the given arguments.
         * @tparam Args - Type of the arguments.
         * @param args - Arguments to pass to the constructor.
         * @return Any object containing a new instance of the type.
         */
        template<typename... Args>
        Any invoke(Args &&...args)
        {
            Any arguments[sizeof...(Args) + 1u]{std::forward<Args>(args)...};
            return invoke_internal(arguments, sizeof...(Args));
        }

    private:
        /**
         * @brief Creates a construct object.
         * @param id - ID of the constructor.
         * @param type_id - ID of the type.
         */
        Constructor(uint32_t id, uint32_t type_id);

        /**
         * @brief Internal invoke function, used to invoke the constructor given a list of args.
         * @param args - List of arguments as any objects
         * @param count - Number of arguments provided.
         * @return Any object containing the newly constructed object.
         */
        [[maybe_unused]] Any invoke_internal([[maybe_unused]]Any *args, size_t count) const;

        uint32_t _id; // ID of the constructor.
        uint32_t _type_id; // ID of the type.

    };
}

