#pragma once

#include <iterator>
#include <cstddef>
#include <unordered_map>

#include "../TypeData.h"
#include "../Type.h"

namespace Chroma::Reflection
{
    class type_container
    {
        friend class TypeData;

        friend class Registry;

        friend class Any;

        friend class Handle;

        friend class Type;

        friend class Function;

        friend class Data;

        template<typename T>
        friend
        class TypeFactory;

    public:
        class iterator
        {
            friend class type_container;

            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = Type;
            using pointer = value_type;
            using reference = value_type;

        public:

            iterator &operator++();

            iterator operator++(int);

            friend bool operator==(const iterator &a, const iterator &b);

            friend bool operator!=(const iterator &a, const iterator &b);


            reference operator*() const;

            pointer operator->();

        private:
            iterator() = default;

            explicit iterator(std::unordered_map<uint32_t, TypeInfo>::iterator iterator);

            std::unordered_map<uint32_t, TypeInfo>::iterator itr;
        };

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

    private:
        type_container() = default;
    };
}

