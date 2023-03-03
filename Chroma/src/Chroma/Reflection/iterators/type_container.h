#pragma once

#include <iterator>
#include <cstddef>
#include <unordered_map>

#include "../type_data.h"
#include "../type.h"

namespace Chroma::Reflection
{
    class type_container
    {
        friend class type_data;

        friend class registry;

        friend class any;

        friend class handle;

        friend class type;

        friend class function;

        friend class data;

        template<typename T>
        friend
        class type_factory;

    public:
        class iterator
        {
            friend class type_container;

            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = type;
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

            explicit iterator(std::unordered_map<uint32_t, type_info>::iterator iterator);

            std::unordered_map<uint32_t, type_info>::iterator itr;
        };

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

    private:
        type_container() = default;
    };
}

