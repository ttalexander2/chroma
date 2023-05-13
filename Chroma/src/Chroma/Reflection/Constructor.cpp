#include "chromapch.h"
#include "Constructor.h"

#include "Registry.h"
#include "iterators/constructor_container.h"

namespace Chroma::Reflection
{

    Constructor::Constructor() : _id(0), _type_id(Registry::resolve<void>().id())
    {

    }


    Constructor::Constructor(uint32_t id, uint32_t type_id) : _id(id), _type_id(type_id)
    {

    }

    bool Constructor::valid() const
    {
        return Registry::valid(_type_id)
               && type_data::instance().types[_type_id].constructors.find(_id)
                  != type_data::instance().types[_type_id].constructors.end();
    }

    bool Constructor::operator==(const Constructor &rhs) const
    {
        return _id == rhs._id && _type_id == rhs._type_id;
    }

    uint32_t Constructor::id() const
    {
        return _id;
    }

    Reflection::Type Constructor::type() const
    {
        return Registry::resolve(_type_id);
    }

    size_t Constructor::arity() const
    {
        if (valid())
            return type_data::instance().types[_type_id].constructors[_id].arity;
        return 0;
    }

    Reflection::Type Constructor::args(size_t index) const
    {
        if (valid() && index < arity())
        {
            return Reflection::Type(type_data::instance().types[_type_id].constructors[_id].arg(index));
        }

        return Registry::resolve<void>();
    }

    Any Constructor::invoke_internal(Any *args, size_t count) const
    {
        if (valid())
            return type_data::instance().types[_type_id].constructors[_id].invoke(args);
        return Any{};
    }

}