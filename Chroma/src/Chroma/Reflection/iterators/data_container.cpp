#include "chromapch.h"
#include "data_container.h"

namespace Chroma::Reflection
{
    data_container::data_container(uint32_t type_id) : _type_id(type_id)
    {

    }

    data_container::iterator data_container::begin() const  // NOLINT(readability-make-member-func-const)
    {
        if (!valid())
            return {};
        return data_container::iterator(_type_id, type_data::instance().types[_type_id].data.begin());
    }

    data_container::iterator data_container::end() const // NOLINT(readability-make-member-func-const)
    {
        if (!valid())
            return {};
        return data_container::iterator(_type_id, type_data::instance().types[_type_id].data.end());
    }

    bool data_container::valid() const
    {
        return type_data::instance().types.find(_type_id) != type_data::instance().types.end();
    }

    data_container::iterator::iterator(uint32_t id, std::unordered_map<unsigned int, data_info>::iterator iterator)
            : itr(iterator), _type_id(id)
    {
    }

    data_container::iterator data_container::iterator::operator++(int) // NOLINT(cert-dcl21-cpp)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    data_container::iterator &data_container::iterator::operator++()
    {
        itr++;
        return *this;
    }

    bool operator==(const data_container::iterator &a, const data_container::iterator &b)
    {
        return a.itr == b.itr;
    }

    bool operator!=(const data_container::iterator &a, const data_container::iterator &b)
    {
        return a.itr != b.itr;
    }

    data_container::iterator::reference data_container::iterator::operator*() const
    {
        return Data(itr->first, _type_id);
    }

    data_container::iterator::pointer data_container::iterator::operator->()
    {
        return Data(itr->first, _type_id);
    }
}