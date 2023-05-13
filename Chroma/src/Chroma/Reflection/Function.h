#pragma once

#include <cstdint>
#include "Type.h"
#include "Any.h"

namespace Chroma::Reflection
{
    // Forward declarations
    class type_data;
    class Registry;
    struct func_info;
    struct type_info;
    class argument_container;
    template<typename T>
    class type_factory;


    /**
     * @brief Class representing a function contained in a type.
     */
    class Function
    {
        // Friend classes
        friend class type_data;
        friend class Registry;
        friend class Any;
        friend class Handle;
        friend class Type;
        friend class function_container;
        template<typename T>
        friend class type_factory;

    public:
        /**
         * @brief Creates an empty function object.
         */
        Function();

        /**
         * @brief Default copy constructor.
         */
        Function(const Function &) = default;

        /**
         * @brief Equality comparable operator.
         * @param rhs - Other function to compare this function to.
         * @return Returns true if both functions are the same, and belong to the same type. False otherwise.
         */
        bool operator==(const Function &rhs) const;

        /**
         * @brief Checks whether the function object represents a valid, registered function.
         * @return Returns true if the function is valid and belongs to a valid type, false otherwise.
         */
        [[nodiscard]] bool valid() const;

        /**
         * @brief Boolean operator.
         * @return Returns true if the function is valid, false otherwise.
         */
        explicit operator bool() const
        { return valid(); }

        /**
         * @brief Gets the ID of this function.
         * @return 32-bit identifier for the function.
         */
        [[nodiscard]] uint32_t id() const;

        /**
         * @brief Gets the provided name for this function.
         * @return Human readable string.
         */
        [[nodiscard]] std::string name() const;

        /**
         * @brief Gets the containing type of this function.
         * @return Type object representing the type.
         */
        [[nodiscard]] Reflection::Type type() const;

        /**
         * @brief Gets the arity of the function.
         * @return Number of arguments the function takes.
         */
        [[nodiscard]] size_t arity() const;

        /**
         * @brief Gets the return type of the function.
         * @return Type object for the return type.
         */
        [[nodiscard]] Reflection::Type return_type() const;

        /**
         * @brief Gets the type of an argument for the function.
         * @param index - Index of the argument to retrieve.
         * @return Type object representing the type of the argument.
         */
        [[nodiscard]] Reflection::Type args(size_t index) const;

        /**
         * @brief Gets an iterable container containing all of the arguments for a function.
         * @return Arguments taken by the function.
         */
        [[nodiscard]] argument_container args() const;

        /**
         * @brief Invokes the function on an object with the provided arguments.
         * @tparam Args - Type of the arguments.
         * @param handle - Object to invoke the function on.
         * @param args - Arguments to pass to the function.
         * @return Result of the function contained in an any object.
         */
        template<typename... Args>
        [[maybe_unused]] Any invoke([[maybe_unused]] Reflection::Handle handle, Args &&...args)
        {
            Any arguments[sizeof...(Args) + 1u]{std::forward<Args>(args)...};
            return invoke_internal(std::move(handle), arguments, sizeof...(Args));
        }

        [[nodiscard]] Any user_data(const std::string& key) const;

    private:
        /**
         * @brief Constructs a function object from the given function ID and type ID.
         * @param id
         * @param type_id
         */
        explicit Function(uint32_t id, uint32_t type_id);

        /**
         * @brief Internal function to invoke the function given a handle and list of args.
         * @param args
         * @param count
         * @return
         */
        [[maybe_unused]] Any invoke_internal([[maybe_unused]] Reflection::Handle, Any *args, size_t count) const;

        uint32_t _id; // Function ID
        uint32_t _type_id; // Type ID
    };
}
