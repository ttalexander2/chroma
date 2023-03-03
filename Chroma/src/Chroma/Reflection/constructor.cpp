#include "chromapch.h"
#include "constructor.h"

#include "registry.h"
#include "iterators/constructor_container.h"

namespace Chroma::Reflection
{

    constructor::constructor() : _id(0), _type_id(registry::resolve<void>().id())
    {

    }


    constructor::constructor(uint32_t id, uint32_t type_id) : _id(id), _type_id(type_id)
    {

    }

    bool constructor::valid() const
    {
        return registry::valid(_type_id)
               && type_data::instance().types[_type_id].constructors.find(_id)
                  != type_data::instance().types[_type_id].constructors.end();
    }

    bool constructor::operator==(const constructor &rhs) const
    {
        return _id == rhs._id && _type_id == rhs._type_id;
    }

    uint32_t constructor::id() const
    {
        return _id;
    }

    Reflection::type constructor::type() const
    {
        return registry::resolve(_type_id);
    }

    size_t constructor::arity() const
    {
        if (valid())
            return type_data::instance().types[_type_id].constructors[_id].arity;
        return 0;
    }

    Reflection::type constructor::args(size_t index) const
    {
        if (valid() && index < arity())
        {
            return Reflection::type(type_data::instance().types[_type_id].constructors[_id].arg(index));
        }

        return registry::resolve<void>();
    }

    any constructor::invoke_internal(any *args, size_t count) const
    {
        if (valid())
            return type_data::instance().types[_type_id].constructors[_id].invoke(args);
        return any{};
    }

}