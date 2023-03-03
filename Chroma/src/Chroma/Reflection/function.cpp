#include "chromapch.h"
#include "function.h"

#include "type_data.h"
#include "registry.h"
#include "iterators/argument_container.h"

namespace Chroma::Reflection
{

    function::function() : _id(0), _type_id(registry::resolve<void>().id())
    {

    }


    function::function(uint32_t id, uint32_t type_id) : _id(id), _type_id(type_id)
    {

    }

    bool function::valid() const
    {
        return registry::valid(_type_id)
               && type_data::instance().types[_type_id].functions.find(_id)
                  != type_data::instance().types[_type_id].functions.end();
    }

    bool function::operator==(const function &rhs) const
    {
        return _id == rhs._id && _type_id == rhs._type_id;
    }

    uint32_t function::id() const
    {
        return _id;
    }

    std::string function::name() const
    {
        if (valid())
            return type_data::instance().types[_type_id].functions[_id].name;
        return std::string{};
    }

    Reflection::type function::type() const
    {
        return registry::resolve(_type_id);
    }

    size_t function::arity() const
    {
        if (valid())
            return type_data::instance().types[_type_id].functions[_id].arity;
        return 0;
    }

    Reflection::type function::return_type() const
    {
        if (valid())
            return Reflection::type(type_data::instance().types[_type_id].functions[_id].return_type);
        return registry::resolve<void>();
    }

    Reflection::type function::args(size_t index) const
    {
        if (valid() && index < arity())
            return Reflection::type(type_data::instance().types[_type_id].functions[_id].arg(index));
        return registry::resolve<void>();
    }

    any function::invoke_internal(Reflection::handle handle, any *args, size_t count) const
    {
        if (valid())
            return type_data::instance().types[_type_id].functions[_id].invoke(std::move(handle), args);
        return any{};
    }

    argument_container function::args() const
    {
        return argument_container(_type_id, _id);
    }

    any function::user_data(const std::string& key) const
    {
        uint32_t hash = basic_hash<uint32_t>::hash(key);
        if (valid())
        {
            return type_data::instance().types[_type_id].functions[_id].user_data[hash];
        }
        return any{};
    }
}