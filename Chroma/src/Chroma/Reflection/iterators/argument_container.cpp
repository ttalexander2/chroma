#include "chromapch.h"
#include "argument_container.h"

namespace Chroma::Reflection
{

    argument_container::iterator::iterator(uint32_t type_id, uint32_t func_id, size_t arity)
            : _type_id(type_id), _func_id(func_id), arity(arity)
    {

    }

    argument_container::iterator &argument_container::iterator::operator++()
    {
        index++;
        return *this;
    }

    argument_container::iterator argument_container::iterator::operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const argument_container::iterator &a, const argument_container::iterator &b)
    {
        return a._type_id == b._type_id && a._func_id == b._func_id && a.index == b.index;
    }

    bool operator!=(const argument_container::iterator &a, const argument_container::iterator &b)
    {
        return !operator==(a, b);
    }

    argument_container::iterator::reference argument_container::iterator::operator*() const
    {
        return Type(type_data::instance().types[_type_id].functions[_func_id].arg(index));
    }

    argument_container::iterator::pointer argument_container::iterator::operator->() const
    {
        return Type(type_data::instance().types[_type_id].functions[_func_id].arg(index));
    }

    argument_container::argument_container(uint32_t type_id, uint32_t func_id)
            : _type_id(type_id), _func_id(func_id)
    {

    }

    bool argument_container::valid() const
    {
        return type_data::instance().types.find(_type_id) != type_data::instance().types.end()
               && type_data::instance().types[_type_id].functions.find(_func_id)
                  != type_data::instance().types[_type_id].functions.end();
    }

    argument_container::iterator argument_container::begin() const
    {
        if (!valid())
            return {};
        return argument_container::iterator(_type_id, _func_id, arity());
    }

    argument_container::iterator argument_container::end() const
    {
        if (!valid())
            return {};
        auto it = argument_container::iterator(_type_id, _func_id, arity());
        it.index = arity();
        return it;
    }

    size_t argument_container::arity() const
    {
        if (!valid())
            return 0;
        return type_data::instance().types[_type_id].functions[_func_id].arity;
    }
}