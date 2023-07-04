#pragma once

#include <cstdint>

namespace Chroma::Reflection
{
    using TypeId = uint32_t;

    namespace Internal
    {

        namespace Util
        {
            struct TypeHash final
            {
                [[nodiscard]] static TypeId next() noexcept
                {
                    static TypeId value{};
                    return value++;
                }
            };
        }

        template<typename T, typename = void>
        struct TypeIndex final
        {
            static TypeId value() noexcept
            {
                static const TypeId value = Util::TypeHash::next();
                return value;
            }
        };

        template<typename T, typename = void>
        struct TypeHash final
        {
            [[nodiscard]] static constexpr TypeId value() noexcept
            {
                return TypeIndex<std::decay_t<T>>::value();
            }
        };

        template<typename T, typename = void>
        inline TypeId type_hash_v = TypeHash<T>::value();

        struct constructor_counter final
        {
            [[nodiscard]] static TypeId next() noexcept
            {
                static TypeId value{};
                return value++;
            }
        };
    }
}

