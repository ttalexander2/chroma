#pragma once

#include <iterator>
#include <cstddef>
#include <unordered_map>

#include "../type_data.h"
#include "../function.h"

namespace Chroma::Reflection
{

    class argument_container
    {
        friend class type_data;
        friend class registry;
        friend class any;
        friend class handle;
        friend class type;
        friend class function;

        template<typename T>
        friend class type_factory;

    public:
        class iterator
        {
            friend class argument_container;

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

            pointer operator->() const;

        private:
            iterator() = default;

            explicit iterator(uint32_t type, uint32_t func, size_t arity);

            std::unordered_map<uint32_t, data_info>::iterator itr;
            uint32_t _type_id{};
            uint32_t _func_id{};
            size_t index{};
            size_t arity{};
        };

        [[nodiscard]] bool valid() const;

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

        [[nodiscard]] size_t arity() const;

    private:
        explicit argument_container(uint32_t type_id, uint32_t func_id);

        uint32_t _type_id{};
        uint32_t _func_id{};
    };
}

