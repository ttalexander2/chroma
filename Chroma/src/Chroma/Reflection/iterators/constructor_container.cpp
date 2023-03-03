#include "chromapch.h"
#include "constructor_container.h"

namespace Chroma::Reflection
{
    constructor_container::constructor_container(uint32_t type_id) : _type_id(type_id)
    {

    }

    constructor_container::iterator constructor_container::begin() const  // NOLINT(readability-make-member-ctor-const)
    {
        if (!valid())
            return {};
        return constructor_container::iterator(_type_id, type_data::instance().types[_type_id].constructors.begin());
    }

    constructor_container::iterator constructor_container::end() const // NOLINT(readability-make-member-ctor-const)
    {
        if (!valid())
            return {};
        return constructor_container::iterator(_type_id, type_data::instance().types[_type_id].constructors.end());
    }

    bool constructor_container::valid() const
    {
        return type_data::instance().types.find(_type_id) != type_data::instance().types.end();
    }

    constructor_container::iterator::iterator(uint32_t id, std::unordered_map<uint32_t, ctor_info>::iterator iterator)
            : itr(iterator), _type_id(id)
    {
    }

    constructor_container::iterator constructor_container::iterator::operator++(int) // NOLINT(cert-dcl21-cpp)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    constructor_container::iterator &constructor_container::iterator::operator++()
    {
        itr++;
        return *this;
    }

    bool operator==(const constructor_container::iterator &a, const constructor_container::iterator &b)
    {
        return a.itr == b.itr;
    }

    bool operator!=(const constructor_container::iterator &a, const constructor_container::iterator &b)
    {
        return a.itr != b.itr;
    }

    constructor_container::iterator::reference constructor_container::iterator::operator*() const
    {
        return {itr->first, _type_id};
    }

    constructor_container::iterator::pointer constructor_container::iterator::operator->() const
    {
        return {itr->first, _type_id};
    }
}