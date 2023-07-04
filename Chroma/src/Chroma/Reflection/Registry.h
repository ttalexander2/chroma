#pragma once

#include <iostream>
#include "Type.h"
#include "Type.inl"
#include "TypeData.h"
#include "TypeHash.h"
#include "TypeFactory.h"


namespace Chroma::Reflection
{
    // Forward declarations
    class type_container;

    /**
     * Class for registering and resolving types within the reflection system.
     */
    class Registry
    {
    public:
        using id_hash = BasicHash<uint32_t>;

        /**
         * @brief Registers a type to the reflection system.
         * @tparam T - Type to register.
         * @return type_factory object used to provide metadata about the type.
         */
        template<typename T>
        static TypeFactory<T> register_type()
        {
            return TypeFactory<T>();
        }

        /**
         * @brief Registers a type to the reflection system under the specified name.
         * @tparam T - Type to register.
         * @param name - User provided identifier for the type.
         * @return type_factory object used to provide metadata about the type.
         */
        template<typename T>
        static TypeFactory<T> register_type(const std::string &name)
        {
            return TypeFactory<T>(name);
        }

        /**
         * @brief Resolves the user provided name to its associated type.
         * @param name - Name of the type
         * @return The type object with information about the type. If the symbol could not be found,
         * the type object will be invalid. Use the type.valid() to check whether the type is valid.
         */
        static Type resolve(const std::string &name) noexcept;

        /**
        * @brief Resolves the provided ID to its associated type.
        * @param id - ID of the type.
        * @return The type object with information about the type. If the symbol could not be found,
        * the type object will be invalid. Use the type.valid() to check whether the type is valid.
        */
        static Type resolve(uint32_t id) noexcept;

        /**
         * @brief Gets an iterable container containing all of the types registered in the system.
         * @return
         */
        static type_container resolve() noexcept;

        /**
         * @brief Gets meta information about the provided type.
         * @tparam T - Type to retrieve information for.
         * @return The type object with information about the type. If the symbol could not be found,
         * the type object will be invalid. Use the type.valid() to check whether the type is valid.
         */
        template<typename T>
        static Type resolve() noexcept
        {
            uint32_t hash = Internal::TypeHash<std::decay_t<T>>::value();
            auto val = TypeData::instance().types.find(hash);
            if (val == TypeData::instance().types.end())
            {
                auto factory = TypeFactory<T>(typeid(T).name());
                return factory._type;
            }
            return Type(val->first);
        }

        /**
         * @brief Checks whether a type has been registered and associated with the provided name.
         * @param name - Name of the type.
         * @return Returns true if the type has been registered with the reflection system under the provided name, false otherwise.
         */
        static bool valid(const std::string &name);

        /**
         * @brief Checks whether a type has been registered and associated with the provided id.
         * @param id - ID of the type.
         * @return Returns true if the type has been registered with the reflection system under the provided ID, false otherwise.
         */
        static bool valid(uint32_t id);

    private:
        /**
         * @brief Gets the ID for a type from the provided name.
         * @param name - Name of the type.
         * @return Returns the id if the type exists in the reflection system under the provided name, otherwise returns 0xFFFFFFFF
         */
        static uint32_t id_from_name(const std::string &name);
    };

} // mirr
