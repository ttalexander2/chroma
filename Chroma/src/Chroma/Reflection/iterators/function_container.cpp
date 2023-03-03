#include "chromapch.h"
#include "function_container.h"

namespace Chroma::Reflection
{
    function_container::function_container(uint32_t type_id) : _type_id(type_id)
    {

    }

    function_container::iterator function_container::begin() const  // NOLINT(readability-make-member-func-const)
    {
        if (!valid())
            return {};
        return function_container::iterator(_type_id, type_data::instance().types[_type_id].functions.begin());
    }

    function_container::iterator function_container::end() const // NOLINT(readability-make-member-func-const)
    {
        if (!valid())
            return {};
        return function_container::iterator(_type_id, type_data::instance().types[_type_id].functions.end());
    }

    bool function_container::valid() const
    {
        return type_data::instance().types.find(_type_id) != type_data::instance().types.end();
    }

    function_container::iterator::iterator(uint32_t id, std::unordered_map<uint32_t, func_info>::iterator iterator)
            : itr(iterator), _type_id(id)
    {
    }

    function_container::iterator function_container::iterator::operator++(int) // NOLINT(cert-dcl21-cpp)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    function_container::iterator &function_container::iterator::operator++()
    {
        itr++;
        return *this;
    }

    bool operator==(const function_container::iterator &a, const function_container::iterator &b)
    {
        return a.itr == b.itr;
    }

    bool operator!=(const function_container::iterator &a, const function_container::iterator &b)
    {
        return a.itr != b.itr;
    }

    function_container::iterator::reference function_container::iterator::operator*() const
    {
        return function(itr->first, _type_id);
    }

    function_container::iterator::pointer function_container::iterator::operator->() const
    {
        return function(itr->first, _type_id);
    }
}