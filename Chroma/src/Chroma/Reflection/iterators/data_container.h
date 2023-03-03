#pragma once

#include <iterator>
#include <cstddef>
#include <unordered_map>

#include "../type_data.h"
#include "../data.h"

namespace Chroma::Reflection
{

    class data_container
    {
        friend class type_data;

        friend class registry;

        friend class any;

        friend class handle;

        friend class type;

        template<typename T>
        friend
        class type_factory;

    public:
        class iterator
        {
            friend class data_container;

            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = data;
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

            explicit iterator(uint32_t id, std::unordered_map<uint32_t, data_info>::iterator iterator);

            std::unordered_map<uint32_t, data_info>::iterator itr;
            uint32_t _type_id{};
        };

        [[nodiscard]] bool valid() const;

        [[nodiscard]] iterator begin() const;

        [[nodiscard]] iterator end() const;

    private:
        explicit data_container(uint32_t type_id);

        uint32_t _type_id{};
    };
}
