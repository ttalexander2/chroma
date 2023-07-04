#pragma once

#include <iterator>
#include <cstddef>
#include <unordered_map>

#include "../TypeData.h"
#include "../Constructor.h"

namespace Chroma::Reflection
{
    class constructor_container
    {
        friend class type_data;

        friend class Registry;

        friend class Any;

        friend class Handle;

        friend class Type;

        friend class Constructor;

        template<typename T>
        friend
        class TypeFactory;

    public:
        class iterator
        {
            friend class constructor_container;

            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = Constructor;
            using pointer = value_type;
            using reference = value_type;

        public:
            iterator &operator++();

            iterator operator++(int);

            friend bool operator==(const iterator &a, const iterator &b);

            friend bool operator!=(const iterator &a, const iterator &b);

            reference operator*() const;

            pointer operator->() const;

        private:
            iterator() = default;

            explicit iterator(uint32_t id, std::unordered_map<uint32_t, CtorInfo>::iterator iterator);

            std::unordered_map<uint32_t, CtorInfo>::iterator itr;
            uint32_t _type_id{};

        };

        [[nodiscard]] bool valid() const;

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

        [[nodiscard]] size_t arity() const;

    private:
        explicit constructor_container(uint32_t type_id);

        uint32_t _type_id{};
    };

}

