#include "chromapch.h"
#include "type_container.h"

namespace Chroma::Reflection
{

    type_container::iterator type_container::begin() const  // NOLINT(readability-make-member-func-const)
    {
        return type_container::iterator(type_data::instance().types.begin());
    }

    type_container::iterator type_container::end() const // NOLINT(readability-make-member-func-const)
    {
        return type_container::iterator(type_data::instance().types.end());
    }

    type_container::iterator::iterator(std::unordered_map<uint32_t, type_info>::iterator iterator)
            : itr(iterator)
    {
    }

    type_container::iterator type_container::iterator::operator++(int) // NOLINT(cert-dcl21-cpp)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    type_container::iterator &type_container::iterator::operator++()
    {
        itr++;
        return *this;
    }

    bool operator==(const type_container::iterator &a, const type_container::iterator &b)
    {
        return a.itr == b.itr;
    }

    bool operator!=(const type_container::iterator &a, const type_container::iterator &b)
    {
        return a.itr != b.itr;
    }

    type_container::iterator::reference type_container::iterator::operator*() const
    {
        return Type(itr->first);
    }

    type_container::iterator::pointer type_container::iterator::operator->()
    {
        return Type(itr->first);
    }
}